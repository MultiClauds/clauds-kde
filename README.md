<p align="center">
  <img src="docs/images/icon.png" width="128" height="128" alt="Clauds icon">
</p>

<h1 align="center">Clauds for KDE Plasma</h1>

<p align="center">
  A KDE Plasma system tray app that switches the Claude desktop app for Linux between accounts.<br>
  Every account stays signed in.
</p>

<p align="center"><a href="https://products.lemche.net/clauds-kde/"><strong>products.lemche.net/clauds-kde</strong></a></p>

<p align="center">
  <a href="https://github.com/LemcheNET/clauds-kde/actions/workflows/kde.yml"><img src="https://github.com/LemcheNET/clauds-kde/actions/workflows/kde.yml/badge.svg" alt="KDE port build"></a>
  <a href="https://github.com/LemcheNET/clauds-kde/releases/latest"><img src="https://img.shields.io/github/v/release/LemcheNET/clauds-kde?label=release" alt="Latest release"></a>
  <a href="https://github.com/LemcheNET/clauds-kde/actions/workflows/codeql.yml"><img src="https://github.com/LemcheNET/clauds-kde/actions/workflows/codeql.yml/badge.svg" alt="CodeQL"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="MIT License"></a>
  <img src="https://img.shields.io/badge/KDE%20Plasma-6-1d99f3.svg" alt="KDE Plasma 6">
  <img src="https://img.shields.io/badge/Qt-6-41cd52.svg" alt="Qt 6">
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599c.svg" alt="C++20">
</p>

<p align="center">
  <img src="kde/docs/panel.png" width="380" alt="The Clauds panel on KDE Plasma with three fake accounts">
</p>

> [!IMPORTANT]
> Clauds is an independent project. It is not made, endorsed, or supported by Anthropic.
> "Claude" is a trademark of Anthropic, PBC.

> [!NOTE]
> Clauds for KDE Plasma is a port of [Clauds](https://github.com/codeFi/clauds), the macOS
> menu bar app by [Razvan Tanase](https://github.com/codeFi). This repository holds only the port.
> For the macOS app, use the [original repository](https://github.com/codeFi/clauds). See
> [Relation to the macOS app](#relation-to-the-macos-app).

## Contents

- [Why](#why)
- [Features](#features)
- [Requirements](#requirements)
- [Install](#install)
- [First use](#first-use)
- [How to use it](#how-to-use-it)
- [Privacy and security](#privacy-and-security)
- [Limits](#limits)
- [Troubleshooting](#troubleshooting)
- [Uninstall](#uninstall)
- [How it works](#how-it-works)
- [Development](#development)
- [Contributing](#contributing)
- [Relation to the macOS app](#relation-to-the-macos-app)
- [Third-party software](#third-party-software)
- [License](#license)

## Why

The Claude desktop app holds one login at a time. If you have a work account and a personal
account, you must sign out and sign in again each time, and you wait for an email code each time.

Clauds keeps one Claude data folder for each account. A switch takes one click. No account
is signed out.

## Features

- **One-click switch.** Click an account in the panel, or in the quick menu of the tray icon.
  Claude opens again with that account.
- **Side by Side.** Run one Claude copy for each account, all at the same time. No restart.
- **Shared Code sessions.** Continue the same Code session from a different account. There are no
  copies: all accounts use the same session files.
- **Session owners.** The app records which account made each session. In Side by Side, each
  account sees only its own sessions (unless you turn on sharing there, which is risky).
- **Auto-switch near a limit.** When the current account is at 90% of a limit, the app goes to the
  account with the most room. It asks first, or switches by itself. Off by default.
- **Live usage.** See the 5-hour limit, the weekly limit, and the weekly per-model limit for each
  account, each with its reset time.
- **Chat windows.** Open claude.ai for any account in its own window, with its own login.
- **Start at Login.** An autostart entry that you can see in System Settings.
- **Native on Plasma.** A StatusNotifierItem tray icon, KDE notifications, and the colors of your
  color scheme. Nothing is hard-coded.
- **Local only.** No server, no analytics, no telemetry, no update check. The app does not read
  your tokens, cookies, or passwords.

## Requirements

- KDE Plasma 6 (Kubuntu 25.04 or later; built for 26.04). On other desktops, the app uses the
  plain Qt tray icon.
- [Claude Desktop for Linux](https://claude.com/download) (the `claude-desktop` package), or the
  unofficial `claude-desktop-unofficial` build. Both keep their data in `~/.config/Claude`.
- To build: CMake 3.22 or later, a C++20 compiler, Qt 6.4 or later, and KDE Frameworks 6.
  `make deps` installs them on Kubuntu.

## Install

> [!WARNING]
> This is an early version (0.x). Make a [backup](#development) before the first switch.

Every merge to `main` makes a release with a Flatpak. You can also build the package on your computer.
This takes a few minutes.

**Flatpak, from a release** (experimental): download
`io.github.multiclauds.clauds-kde-<version>.flatpak` from the
[latest release](https://github.com/LemcheNET/clauds-kde/releases/latest), then:

```bash
flatpak install --user io.github.multiclauds.clauds-kde-<version>.flatpak
flatpak run io.github.multiclauds.clauds-kde
```

The Flatpak needs the KDE runtime (`org.kde.Platform//6.10`) from Flathub. It finds the Claude
desktop app that is installed on your computer (for example the `.deb` package) and starts and quits
it on the host. It works on your real `~/.config/Claude` and `~/.local/share/clauds`.

**Build from source:**

```bash
git clone https://github.com/LemcheNET/clauds-kde.git
cd clauds-kde
make deps       # build dependencies (sudo apt)
make test       # configure, build, run the test suites
make install    # build the .deb and install it (sudo apt)
```

Then start **Clauds** from the application launcher. The icon appears in the system tray.

| Package | Command | State |
| --- | --- | --- |
| `.deb` | `make deb`, or `make install` to also install it | The supported way. CI builds it on Ubuntu 26.04. |
| Flatpak | A release download, or `make flatpak-deps`, then `make flatpak-install` | Experimental. See [kde/packaging/flatpak](kde/packaging/flatpak). `make flatpak` writes `build-flatpak/io.github.multiclauds.clauds-kde.flatpak`. |

`make help` lists every target. The version number follows
[Semantic Versioning](https://semver.org/spec/v2.0.0.html). See
[CONTRIBUTING.md](CONTRIBUTING.md#versions-and-releases).

### Updates

The app does not update itself, and it does not look for new versions. To update:

```bash
cd clauds-kde
git pull
make install
```

For the Flatpak, install the bundle of the newer release the same way as the first time.

An update keeps your accounts and logins. Quit Clauds before you install, and start it again after.

## First use

1. Open the Claude desktop app and sign in to your first account, as usual.
2. Open Clauds. At the first start, it asks if it can start at login. Choose what you want.
   Then click the Clauds icon in the system tray.
3. Click **Save Current Claude Login as Account…** and give it a name. No files move.
4. Click **Add Account** and give the second account a name.
5. A sign-in window opens. Sign in to the second account there. This login is for the live usage
   numbers and the chat window. The window closes by itself when the login is good.
6. Claude opens for the new account. Sign in there too. This login is for the Claude app.

You do steps 5 and 6 one time for each account. After that, both logins stay.

> [!NOTE]
> Why two sign-ins? The Claude app keeps its login in its own encrypted store. Clauds does
> not read that store. So the Claude app and the Clauds web views each need a login.

## How to use it

### The tray icon

| Click | What opens |
| --- | --- |
| Left click | The panel, with a card for each account. |
| Right click | A quick menu with every account, for a switch with one click. |

On Wayland, KWin decides where the panel opens. The quick menu always opens at the icon.

### Switch accounts (restart mode)

Click an account. Claude quits, and opens again with that account. This takes some seconds.

Running Code sessions stop when Claude quits. Finish or pause your work first.

### Side by Side

Turn on **Side by Side (no restart)**.

- Each account gets its own Claude copy. All copies can be open at the same time.
- A click on an account starts its copy.
- A green dot shows which accounts have a copy open.
- Each account sees only the Code sessions that it owns.

When you turn this switch on or off, all Claude copies quit one time.

### Share Code sessions

Turn on **Share Code Sessions**.

- All accounts in the same organization see the same Code sessions.
- You can start a session with one account and continue it with a different account.
- For a new account: sign in, then use **More > Share Code Sessions Now (Restarts Claude)** one time.

#### Share Code sessions in Side by Side (risky)

In Side by Side, this switch is off by default. You can turn it on. The app shows a warning first.

With this on, all open Claude copies write the same session files. This can go wrong:

- If the same session is open in two accounts, it can lose messages, split in two, or go back to
  an old state.
- A session that you archive or rename in one copy can come back or change in a different copy.
- A new session can stay hidden in the other copies until they restart.
- The app cannot know which account made a new session. It gives new sessions to the main account.

Rule to stay safe: work on one session in one account at a time.

The conversation text of each session is in `~/.claude/projects`. The app does not change those files.

### Auto-switch near a limit

Open **More > Auto-Switch Near a Limit**. This works in restart mode only (Side by Side off).

| Choice | What the app does |
| --- | --- |
| **Off** | Nothing. This is the default. |
| **Ask Me First** | Shows a question. You click **Switch** or **Cancel**. |
| **Switch Automatically** | Switches without a question. |

The rules:

- The current account must be at 90% or more of one limit (5-hour, weekly, or weekly per-model).
- A different account must be at 80% or less on all its limits. The app picks the one with the most room.
- The app uses only live numbers that are less than 15 minutes old. So the other accounts must be
  signed in for live usage.
- The app waits 20 minutes between two questions or two switches.
- Claude must be open. The app does not start Claude for an auto-switch.

A switch quits Claude. Running Code sessions in the app stop. With **Switch Automatically**, this
can happen in the middle of your work. Use **Ask Me First** if that is a problem.

### Usage numbers

Each account card shows up to three limits:

| Label | Limit | Reset text |
| --- | --- | --- |
| `5h` | The 5-hour limit | Time left, for example `1h 03m left` |
| `Week` | The weekly limit for all models | Day and time, for example `Thu 14:00` |
| `<Model> wk` | The weekly limit for one model | Day and time |

- The color of a number shows how near the limit is. The colors come from your color scheme.
- Put the pointer on a limit to see its full name and its exact reset time.
- The numbers refresh every 2 minutes.

When there are no live numbers, the card says why:

| Text on the card | What it means | What to do |
| --- | --- | --- |
| Sign in for live usage | The login for live usage is missing. | Use **Sign In for Live Usage…** in the account menu. |
| Live usage is not available: this account is not allowed to read usage and billing information | The login works, but claude.ai refuses the usage numbers for this account. | Ask an admin of the organization for access. |
| No 5-hour or weekly limits to show | claude.ai answers, but reports no such limits for this account. | Nothing. Some plans have no such limits. |
| Could not load new usage numbers | No connection, or claude.ai gave an answer that the app does not know. | Use **Load Usage Again**. If it stays, open an issue. |

### The account menu (the button on a card)

| Item | What it does |
| --- | --- |
| Load Usage Again | Reads the live usage numbers again. Shown only after a failed read. |
| Sign In for Live Usage… | Opens the sign-in window. Shown only when the login is missing or a read failed. |
| Open Chats in Window | Opens claude.ai for this account in its own window. |
| New Chat in Window | Opens a new chat for this account. |
| Rename… / Set Email… | Changes the text on the card. The email is only a label. |
| Move Up / Move Down | Changes the order. |
| Remove… | Moves the account's Claude data to the Trash. Its sessions go to the shared pool first. |

### The app menu (More, at the bottom)

| Item | What it does |
| --- | --- |
| Share Code Sessions Now (Restarts Claude) | Links the session folders now. |
| Start at Login | Starts Clauds when you log in. The app asks one time, at the first start. It does not turn this on without your yes. |
| Auto-Switch Near a Limit | See [Auto-switch near a limit](#auto-switch-near-a-limit). |
| Show Saved Accounts in File Manager | Opens the folder with the saved account data. |
| Open Claude | Opens the Claude app. |
| About Clauds | Shows the version, the license, and the project link. |
| Quit Clauds | Quits the tray app. Claude stays open. |

## Privacy and security

- **Everything stays on your computer.** There is no server, no account, no analytics, and no telemetry.
- **One network use.** The app contacts `claude.ai`, through Qt WebEngine, for the chat windows and
  the live usage numbers. It contacts no other host. It does not look for updates.
- **No secrets are read.** The app moves whole Claude data folders. It does not open, copy, or store
  passwords, tokens, or cookies, and it does not read the encrypted login store of the Claude app.
- **Private files.** All files and folders that the app makes can be opened only by your user.
- **Nothing is deleted.** When two versions of a session file exist, the newer one wins and the
  older one is kept in a `_conflicts` folder. A removed account goes to the Trash.
- **No sandbox for the `.deb`.** The app must rename folders in `~/.config` and `~/.local/share`,
  and it must find, quit, and start Claude. Read the code before you run it. The port is about
  3,800 lines.
- **The Flatpak is not a strong sandbox.** For the same reason, it asks for access to the Claude
  data folder and for permission to run commands on the host.

To report a security problem, read [SECURITY.md](SECURITY.md).

## Limits

- **Desktop app only.** The Claude Code CLI (`~/.claude` and its login) is not switched.
- **Unofficial usage numbers.** The live usage numbers come from the same address that the claude.ai
  website uses. It is not a public interface. Anthropic can change it, and then the live numbers
  stop until the app is updated. The app then shows the older numbers that Claude saves on the computer.
- **Claude reads sessions at start.** After a layout change, a Claude copy that is open shows the
  new session list only after its next start.
- **Conversation text is shared by design.** Claude keeps the text of Code sessions in
  `~/.claude/projects` for all accounts. Do not send messages to the same session from two open
  copies at the same time.
- **Artifacts are not shared.** A published artifact belongs to one account on Anthropic's servers.
- **Delete is for all.** With shared sessions, a session that you delete is gone for all accounts.
- **Claude updates can break things.** The app depends on how the Claude desktop app stores its
  data and names its processes. A Claude update can change that.
- **One disk.** `~/.config` and `~/.local/share` must be on the same disk, because a switch renames folders.
- **Your Claude plan terms apply.** Use only accounts that are yours.

## Troubleshooting

**Claude did not quit.**
Close Claude yourself (Ctrl+Q in each copy), then click the account again.

**Clauds says that the Claude desktop app was not found, or does not see a running Claude.**
Find where Claude is, then set `CLAUDS_CLAUDE_LAUNCHER` or `CLAUDS_CLAUDE_INSTALL_DIRS`.
See [Settings for unusual installs](kde/README.md#settings-for-unusual-installs).

**A new account shows a blank window in Side by Side.**
Quit that copy and click the account again. If it stays blank, turn Side by Side off and on.

**The card shows old numbers, or a text in place of the numbers.**
See the table in [Usage numbers](#usage-numbers).

**The session list looks wrong after a mode change.**
Quit and open that Claude copy. Claude reads the session list only at start.

**"Start at Login" does not stay on.**
Open System Settings > Autostart, and look for Clauds. The entry is the file
`~/.config/autostart/io.github.multiclauds.clauds-kde.desktop`.

**The panel opens in the wrong place.**
On Wayland, KWin places the panel. Use the quick menu (right click) to switch at the icon.

**I want to see what the app saved.**
Use **More > Show Saved Accounts in File Manager**, or open `~/.local/share/clauds`.

**Something went wrong with session files.**
Stop. Do not delete anything. Older versions are in
`~/.local/share/clauds/SharedSessions/_conflicts`. Then open an issue, without private data.

## Uninstall

Do these steps in this order. They put the session files back in the Claude folders before the app
goes away.

1. Turn off **Side by Side**.
2. Turn off **Share Code Sessions**. Each account gets its own session files back.
3. Click the account that you want to keep in the Claude app. It becomes the current account.
4. Turn off **More > Start at Login**.
5. Quit Clauds and remove the package: `make uninstall`, or `sudo apt remove clauds`.
   For the Flatpak: `make flatpak-uninstall`, or `flatpak uninstall io.github.multiclauds.clauds-kde`.

The folder `~/.local/share/clauds` still holds the saved data of your other accounts, and sessions
that no account owns. Make a backup of it before you delete it:

```bash
tar -czf ~/clauds-backup.tgz -C ~/.local/share clauds
```

## How it works

In short: Claude keeps its login in `~/.config/Claude`. Clauds keeps one such folder for each
account in `~/.local/share/clauds/Profiles`, and renames them (restart mode) or starts Claude with
a different data folder (Side by Side).

| Read this | For |
| --- | --- |
| [kde/README.md](kde/README.md#folders) | The folders, and the settings for unusual installs |
| [kde/docs/DESIGN.md](kde/docs/DESIGN.md) | The design of the port, the platform mapping, and the open risks |
| [docs/HOW-IT-WORKS.md](docs/HOW-IT-WORKS.md) | The folders, the two ways to switch, the session layouts, the owner record, the data safety rules, and the source files |

## Development

```bash
make            # configure and build
make test       # run the test suites
make run        # start the app from the build folder
make deb        # build the .deb
make flatpak    # build the Flatpak bundle (experimental)
make clean      # delete the build folders (.deb and Flatpak)
```

- The code is in [`kde/`](kde/): `src/core` (Qt Core only, with the tests) and `src/app` (tray,
  panel, web views, KDE integration).
- The version number of the port is in [`kde/CMakeLists.txt`](kde/CMakeLists.txt).
- Without KDE Frameworks 6 (`make KF6=OFF`), the build makes a plain Qt tray app.
- To make the README picture again:
  `QT_QPA_PLATFORM=offscreen build-kde/src/app/clauds --render-panel kde/docs/panel.png`
- More, with the command line hooks for tests: [kde/README.md](kde/README.md#development).

Make a backup before you test changes to the switch or the session code:

```bash
tar -czf ~/claude-backup.tgz -C ~/.config Claude -C ~/.local/share clauds
```

## Contributing

Bug reports, ideas, and pull requests are welcome.

- [CONTRIBUTING.md](CONTRIBUTING.md): how to build, test safely, sign your commits, and send a pull request
- [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md): how we behave
- [SECURITY.md](SECURITY.md): how to report a security problem in private
- [SUPPORT.md](SUPPORT.md): where to ask for help
- [CHANGELOG.md](CHANGELOG.md): what changed in each version

## Relation to the macOS app

Clauds is a macOS menu bar app by [Razvan Tanase](https://github.com/codeFi). This project began as
a fork of it and is now a separate repository. It holds the KDE Plasma port, written in C++ with
Qt 6 and KDE Frameworks 6 (the code is in [`kde/`](kde/)).

- The port keeps the same features, the same words, and the same data safety rules.
- It has no self-update. See [the differences](kde/README.md#use).
- The macOS source is no longer in this repository, and the two projects do not share code. For the
  macOS app, its download, and its documentation, go to the
  [original repository](https://github.com/codeFi/clauds).

## Third-party software

- [Qt 6](https://www.qt.io) (Core, Widgets, Quick, WebEngine), used under the LGPL v3. The app
  links to the Qt libraries of your system.
- [KDE Frameworks 6](https://develop.kde.org/products/frameworks/) (StatusNotifierItem,
  Notifications, CoreAddons, ColorScheme), used under the LGPL. The app links to the libraries of
  your system.

## License

[MIT](LICENSE) © 2026 [Razvan Tanase](https://github.com/codeFi) (Clauds, the macOS app) and the
contributors of the KDE port.

Clauds is not affiliated with Anthropic. "Claude" and "Anthropic" are trademarks of
Anthropic, PBC. You use this software at your own risk.
