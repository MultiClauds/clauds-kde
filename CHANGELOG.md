# Changelog

All notable changes to this project are written in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).
This project uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

The KDE Plasma port, in [`kde/`](kde/). Its version is 0.1.0 and it has no release yet.

### Changed

- Clauds is also in the shared LemcheNET apt repository on https://products.lemche.net/apt,
  which one `lemchenet-archive-keyring` package adds together with its key. Every release
  starts a rebuild of that repository. The Clauds-only repository keeps getting every release.
- Clauds ships only as a `.deb` now. The Flatpak is gone: it pulled in the KDE runtime and the Qt
  WebEngine base, which took far more disk space than the app. Every release attaches `clauds_<version>_amd64.deb` and
  publishes it to a signed apt repository on https://products.lemche.net/clauds-kde/apt, so
  `apt upgrade` installs new versions. `make apt-repo` builds the repository locally.
  To move from the Flatpak: `flatpak uninstall io.github.multiclauds.clauds-kde`, then follow the
  apt steps in the README. Your accounts stay in `~/.local/share/clauds`.

- The Flatpak app ID is now `io.github.multiclauds.clauds-kde` (it was `io.github.atterdag.clauds`).
  The desktop file, the Start at Login entry and the bundle name use it. `make flatpak` writes
  `build-flatpak/io.github.multiclauds.clauds-kde.flatpak`.

### Fixed

- Flatpak: the app now works on the real `~/.local/share/clauds`, `~/.config/Claude` and Trash.
  Flatpak had pointed `XDG_DATA_HOME` and `XDG_CONFIG_HOME` into `~/.var/app/<id>/`.
- Flatpak: on a first start, `~/.local/share/clauds/Profiles` did not exist on the host, so "Show
  Saved Accounts" failed and the saved accounts were lost when the app quit. Flatpak mounts a host
  folder only if it exists; the manifest now has it create `~/.local/share/clauds`, the Trash and
  the autostart folder.
- Flatpak: "Show Saved Accounts" opens the file manager at once through the host. Before, Qt waited
  about 25 s for the portal.

### Added

- Semantic Versioning with automatic releases: every merge to `main` gets the next version from
  the commit titles, a tag `v<version>` and a GitHub release with the Flatpak attached. The version
  is in the file `VERSION`. See `CONTRIBUTING.md`.
- The Flatpak now finds, quits and starts Claude Desktop on the host (for example the `.deb`
  package) through `flatpak-spawn --host`. Before, it only saw its own sandbox. The scan of the
  host uses one `find` call and a 1 s cache, so the panel stays fast.
- KDE Plasma port of Clauds, written in C++ with Qt 6 and KDE Frameworks 6: a StatusNotifierItem
  tray icon, a Qt Quick panel with one card for each Claude account, and the colors of the user's
  color scheme.
- The same features as the macOS app: switch the Claude desktop app between accounts, Side by Side,
  shared Code sessions with a session owner record, auto-switch near a limit, live plan usage,
  chat windows for each account, and Start at Login (an XDG autostart entry).
- Support for the Claude desktop app for Linux: the `claude-desktop` package and the unofficial
  build, with data in `~/.config/Claude`. The process tree of the official package is covered by a
  test.
- Removed accounts go to the freedesktop Trash.
- Messages on an account card that say why there are no live numbers: not signed in, not allowed to
  read usage and billing information, no 5-hour or weekly limits reported, or no connection.
- A `.deb` package (`make deb`) and an experimental Flatpak (`make flatpak`).
- A top-level `Makefile` with the targets `deps`, `build`, `test`, `run`, `deb`, `install`,
  `uninstall`, `flatpak` and `clean`.
- Quality gates on GitHub: a DCO check (every commit needs a `Signed-off-by` line), CodeQL, dependency
  review, OpenSSF Scorecard, and weekly Dependabot updates for the GitHub Actions. Actions are
  pinned by commit.

### Changed

- Clauds ships only as a `.deb` now. The Flatpak is gone: it pulled in the KDE runtime and the Qt
  WebEngine base, which took far more disk space than the app. Every release attaches `clauds_<version>_amd64.deb` and
  publishes it to a signed apt repository on https://products.lemche.net/clauds-kde/apt, so
  `apt upgrade` installs new versions. `make apt-repo` builds the repository locally.
  To move from the Flatpak: `flatpak uninstall io.github.multiclauds.clauds-kde`, then follow the
  apt steps in the README. Your accounts stay in `~/.local/share/clauds`.

- The README, `CONTRIBUTING.md`, `SECURITY.md`, `SUPPORT.md` and the issue and pull request forms
  describe the KDE port. The macOS app is kept as it is upstream.
- A 403 answer for the usage numbers that comes as JSON is shown as "not allowed to read usage",
  not as "sign in".

### Fixed

- Clauds crashed when it quit. The tray menu was deleted before the tray item that uses it.

### Removed

- The macOS source and its scripts (`Sources/`, `Package.swift`, `build.sh`, `release.sh`, the
  Sparkle appcast, the DMG and release-note files, `docs/RELEASING.md`) and its CI build. They live
  in the [original repository](https://github.com/codeFi/clauds).
- The `VERSION` file. The version of the port is in `kde/CMakeLists.txt`.

### Not in the KDE port

- Self-update (Sparkle). Install a new package instead.
- A `.deb` download. Releases carry only the Flatpak (experimental). Build the `.deb` from the source.

## macOS app (upstream)

The entries below are the history of the macOS app, as released by its author. This repository no
longer holds that app. They are kept as the history that the port started from.

## [1.0.0] - 2026-11-16

First release with the name Clauds.

### Added

- Menu bar panel with one card for each Claude account.
- Switch the Claude desktop app between accounts. Each account stays signed in.
- Side by Side mode: one Claude copy for each account, all open at the same time.
- Shared Code sessions between accounts, with a record of the owner of each session.
- Share Code sessions in Side by Side. Off by default, with a warning, because it is risky.
- Live plan usage for each account: 5-hour, weekly, and weekly per-model limits, with reset times.
- Auto-switch near a limit (… menu): off, ask first, or automatic. Off by default.
- Activity indicator while usage numbers load, and a notice when they cannot load.
- Sign-in window for live usage that closes by itself.
- A claude.ai chat window for each account.
- Self-update from GitHub releases, with Sparkle. The app shows the release notes and lets the user
  install, skip, or wait. Each update must have a valid EdDSA signature.
- Start at Login.
- About window.
- Release download as a DMG file (universal, ad-hoc signed, not notarized).
- Data from the first app name ("Claude Switcher") moves to the new folder at the first start
  while Claude is closed.

[Unreleased]: https://github.com/LemcheNET/clauds-kde/commits/main
[1.0.0]: https://github.com/codeFi/clauds/releases/tag/v1.0.0
