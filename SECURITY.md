# Security Policy

Clauds moves the data folders of the Claude desktop app, which hold your logins and
your Code sessions. Security reports are important and welcome.

## Supported versions

Only the newest version on the `main` branch gets security fixes, until there is a first release.

## How to report a vulnerability

**Do not open a public issue for a security problem.**

Use GitHub private vulnerability reporting:

1. Go to <https://github.com/LemcheNET/clauds-kde/security/advisories/new>.
2. Describe the problem, the steps to reproduce it, and the effect.

You can expect a first answer in 7 days. When the fix is released, the report is published with
credit to you, unless you do not want that.

## Do not send private data

Do not attach these files to a report or an issue. They can identify you or your accounts:

- `~/.local/share/clauds/accounts.json`
- `~/.local/share/clauds/SharedSessions/owners.json`
- Anything from `~/.config/Claude`, `~/.local/share/clauds/Profiles`, or
  `~/.local/share/clauds/QtWebEngine`
- A core dump of the app (`coredumpctl`). It can hold account data.

## What the app does and does not do

- All data stays on your computer. The app has no server, no analytics, and no telemetry.
- The app contacts `claude.ai`, through Qt WebEngine, for the chat windows and the live usage
  numbers. It contacts no other host.
- The app does not update itself and does not look for new versions.
- The app does not read, copy, or store passwords, tokens, or cookies. It moves whole Claude data
  folders. The web views keep their own logins, as a browser does.
- The app does not read the encrypted login store of the Claude app.
- The files and folders that the app makes are private to your user (folders `700`, files `600`).
- The `.deb` runs without a sandbox, with the rights of your user. It must rename folders in
  `~/.config` and `~/.local/share`, and it must find, quit, and start the Claude app.
- The Flatpak is not a strong sandbox. It has access to the Claude data folder and permission to
  run commands on the host (`org.freedesktop.Flatpak`), for the same reason.

## About the packages

Every merge to `main` makes a GitHub release with a Flatpak (experimental). The `.deb` has no
download: you build it from the source code, with `make install`. You can also build the Flatpak
with `make flatpak-install`. The build uses the Qt and KDE Frameworks packages of
your distribution.

## Scope

In scope: anything in this repository that can leak, damage, or mix up account data or session
files, or that lets a different local user or process get access to them.

Out of scope: problems in the Claude desktop app or in claude.ai. Report those to Anthropic.
