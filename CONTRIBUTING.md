# Contributing

Thank you for your help. This is a small project. Small, clear changes are the easiest to merge.

## Before you start

- For a bug: open an issue with the bug form.
- For a new feature: open an issue first, so that we can agree on the idea before you write code.
- For a security problem: do not open an issue. Read [SECURITY.md](SECURITY.md).

## Set up

You need Kubuntu 25.04 or later (or another distribution with Qt 6.4+ and KDE Frameworks 6).

```bash
git clone https://github.com/LemcheNET/clauds-kde.git
cd clauds-kde
make deps    # build dependencies (sudo apt)
make test    # configure, build, run the test suites
make run     # start the app from the build folder
```

The code is in `kde/`: `src/core` (Qt Core only, with the tests) and `src/app` (tray, panel, web
views, KDE integration). See [kde/README.md](kde/README.md#development).

This repository has no macOS code. Send changes for the macOS app to the
[original repository](https://github.com/codeFi/clauds).

## Test safely

This app moves real Claude data folders. Be careful when you test.

- Make a backup first:
  `tar -czf ~/claude-backup.tgz -C ~/.config Claude -C ~/.local/share clauds`
- Use the session test hooks on a copy of the data, not on the real folders:
  `clauds --link-sessions <claude data dir> <shared dir> [account tag]` and
  `clauds --unlink-sessions <claude data dir> <shared dir> [account tag]`.
- Run the app on a copy of the data: set `CLAUDS_CLAUDE_DATA_DIR` and `CLAUDS_DATA_DIR` to folders
  of your own. See [kde/README.md](kde/README.md#settings-for-unusual-installs).
- Look at the panel without a display:
  `QT_QPA_PLATFORM=offscreen build-kde/src/app/clauds --render-panel <png>`.

## Rules for the code

- Keep the data safety rules in [docs/HOW-IT-WORKS.md](docs/HOW-IT-WORKS.md). The most important ones:
  move files (do not copy), delete nothing, and stop when the owner record cannot be read.
- Do not add code that reads tokens, cookies, or passwords.
- Do not add network calls to new hosts. The app contacts `claude.ai` only.
- Do not add analytics or telemetry.
- Follow the style of the code near your change. Write short comments that say why.
- Do not hard-code colors. Use the color scheme of the user.
- If you change the panel, make the picture again:
  `QT_QPA_PLATFORM=offscreen build-kde/src/app/clauds --render-panel kde/docs/panel.png`.

## Branches

`main` is the default branch. It is the next version of the port, and every release is a tag
`v<version>` on it.

| Branch | What it is |
| --- | --- |
| `main` | The default branch. It changes only through a pull request. |
| `feature/<name>`, `fix/<name>` | One change. It starts from `main` and goes back to `main`. |
| `develop` | Read-only. It holds the history from before the move to `main`. Do not send pull requests to it. |

Rules that GitHub applies to `main`:

- It changes only through a pull request. The checks (KDE port, DCO, CodeQL) must pass.
- No force push. It cannot be deleted.

Send your pull request to `main`.

## Versions and releases

The version follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html). **Every merge to
`main` is a release.** The workflow [`release.yml`](.github/workflows/release.yml) picks the next
version with [`scripts/next-version.sh`](scripts/next-version.sh), builds the `.deb` with it on
Ubuntu 26.04, creates the tag `v<version>`, attaches `clauds_<version>_amd64.deb` to a GitHub
release and rebuilds the apt repository (the workflow [`pages.yml`](.github/workflows/pages.yml)).

The bump comes from the titles of the commits since the last tag:

| Commit title | Bump |
| --- | --- |
| `feat!: ...`, `fix(tray)!: ...`, or `BREAKING CHANGE:` in the body | major |
| `feat: ...`, `feat(tray): ...` | minor |
| anything else (`fix:`, `docs:`, `kde: ...`) | patch |

While the major version is 0, a breaking change bumps the minor version and a feature bumps the
patch version. To release 1.0.0, write `1.0.0` in the file `VERSION` in a pull request and merge it.
The first release uses the number in `VERSION`.

- The file [`VERSION`](VERSION) is the only place that holds the version. Local builds use it. The
  release workflow writes the release version into it before it builds.
- Put `[skip release]` in the merge commit message to merge without a release.
- Local build of the apt repository: `make apt-repo` writes an unsigned repository to `build-apt/`.

### The apt repository

`pages.yml` deploys `site/` to https://products.lemche.net/clauds-kde/ and builds the apt
repository in `apt/` with [`scripts/build-apt-repo.sh`](scripts/build-apt-repo.sh). It takes the
`.deb` of the last five releases from GitHub, so a deploy of the site alone keeps the repository.

The repository is signed with a key that is used for nothing else. It lives in the repository
secret `APT_SIGNING_KEY` (armored, no passphrase). The workflow publishes the public half as
`apt/clauds-archive-keyring.gpg`. To make the key once:

```bash
export GNUPGHOME=$(mktemp -d)
gpg --batch --pinentry-mode loopback --passphrase '' --quick-gen-key "Clauds apt repository" ed25519 sign 0
gpg --armor --export-secret-keys | gh secret set APT_SIGNING_KEY --repo LemcheNET/clauds-kde
gpg --armor --export > clauds-archive-key.asc   # keep a copy of the public key
rm -rf "$GNUPGHOME"
```

If you replace the key, every user must download the new `clauds-archive-keyring.gpg` once.

## Sign your commits (DCO)

Every commit must have a `Signed-off-by` line with your name and the email address of the commit.
With it, you state that you have the right to send the change under the license of this project
(the [Developer Certificate of Origin](https://developercertificate.org)).

```bash
git commit -s                      # adds the line
git rebase --signoff main          # adds it to commits that you already made
```

A check on each pull request fails when a commit has no such line.

## Checks on a pull request

| Check | What it does |
| --- | --- |
| KDE port | Builds with plain Qt 6 and with KDE Frameworks 6, runs the tests, makes the `.deb` |
| DCO | Every commit has a `Signed-off-by` line |
| CodeQL | Looks for security problems in the C++ code and in the workflows |
| Dependency review | Stops a new dependency that has a known vulnerability |

Dependabot sends pull requests for new versions of the GitHub Actions. OpenSSF Scorecard checks the
supply-chain practices of the repository each week.

## Pull requests

1. Make a branch from `main`: `git switch -c feature/<name> main`.
2. Make sure that `make test` passes and that the build gives no new warnings.
3. Add a line to `CHANGELOG.md` below "Unreleased".
4. Fill in the pull request form. Say how you tested the change.

Never put real account names, emails, account ids, or session files in commits, issues, or pictures.

## License

Your contributions use the [MIT License](LICENSE) of this project.
