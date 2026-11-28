#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
#
# Builds the apt repository for Clauds from .deb files.
#
#   scripts/build-apt-repo.sh <output dir> <file.deb>...
#
# The output dir gets this layout, which apt reads with
#   deb [signed-by=/usr/share/keyrings/clauds-archive-keyring.gpg] <url> stable main
#
#   pool/main/c/clauds/clauds_<version>_<arch>.deb
#   dists/stable/main/binary-<arch>/Packages(.gz)
#   dists/stable/{Release,Release.gpg,InRelease}
#   clauds-archive-keyring.gpg   (binary public key, for /usr/share/keyrings)
#   clauds-archive-keyring.asc   (the same key, armored)
#   clauds.sources               (a deb822 sources file for /etc/apt/sources.list.d)
#
# Set APT_SIGNING_KEY_ID to the ID of a secret key in your GnuPG keyring to sign the
# repository. Without it the repository is not signed, and apt refuses it unless you mark it
# [trusted=yes]. That is only for a local test.
#
# Set APT_REPO_URL to the URL the repository is served at. It goes into clauds.sources.
#
# Needs dpkg-scanpackages (dpkg-dev), apt-ftparchive (apt-utils) and gpg.
set -euo pipefail

if [ $# -lt 2 ]; then
    echo "usage: $0 <output dir> <file.deb>..." >&2
    exit 2
fi

out=$1
shift

suite=stable
component=main
url=${APT_REPO_URL:-https://products.lemche.net/clauds-kde/apt}

pool="$out/pool/$component/c/clauds"
mkdir -p "$pool"
for deb in "$@"; do
    cp -f "$deb" "$pool/"
done

cd "$out"

archs=$(for deb in pool/"$component"/c/clauds/*.deb; do dpkg-deb -f "$deb" Architecture; done | sort -u)
for arch in $archs; do
    dir="dists/$suite/$component/binary-$arch"
    mkdir -p "$dir"
    # --multiversion keeps every version in the index, so apt can also go back to an older one.
    dpkg-scanpackages --multiversion --arch "$arch" pool /dev/null > "$dir/Packages"
    gzip -9nkf "$dir/Packages"
done

rm -f "dists/$suite/Release" "dists/$suite/Release.gpg" "dists/$suite/InRelease"
apt-ftparchive \
    -o APT::FTPArchive::Release::Origin=Clauds \
    -o APT::FTPArchive::Release::Label=Clauds \
    -o APT::FTPArchive::Release::Suite="$suite" \
    -o APT::FTPArchive::Release::Codename="$suite" \
    -o APT::FTPArchive::Release::Components="$component" \
    -o APT::FTPArchive::Release::Architectures="$(echo $archs)" \
    -o APT::FTPArchive::Release::Description="Clauds for KDE Plasma" \
    release "dists/$suite" > "dists/$suite/Release.tmp"
mv "dists/$suite/Release.tmp" "dists/$suite/Release"

if [ -n "${APT_SIGNING_KEY_ID:-}" ]; then
    gpg --batch --yes --local-user "$APT_SIGNING_KEY_ID" --armor --detach-sign \
        --output "dists/$suite/Release.gpg" "dists/$suite/Release"
    gpg --batch --yes --local-user "$APT_SIGNING_KEY_ID" --clearsign \
        --output "dists/$suite/InRelease" "dists/$suite/Release"
    gpg --batch --yes --export "$APT_SIGNING_KEY_ID" > clauds-archive-keyring.gpg
    gpg --batch --yes --armor --export "$APT_SIGNING_KEY_ID" > clauds-archive-keyring.asc
    signed_by="Signed-By: /usr/share/keyrings/clauds-archive-keyring.gpg"
else
    echo "APT_SIGNING_KEY_ID is not set: the repository is NOT signed (local test only)." >&2
    signed_by="Trusted: yes"
fi

cat > clauds.sources <<EOF
Types: deb
URIs: $url
Suites: $suite
Components: $component
$signed_by
EOF

echo "apt repository written to $(pwd)"
