# Clauds for KDE Plasma

A KDE Plasma system tray app that switches the Claude desktop app for Linux between accounts.
Every account stays signed in. This is a port of the macOS app in this repository; it keeps the
same features, the same words, and the same data safety rules.

<p align="center"><img src="docs/panel.png" width="380" alt="The panel with three fake accounts"><br><sub>Fake accounts, drawn with <code>--render-panel</code> in a build container (plain Qt style). On Plasma it uses your widget style and color scheme.</sub></p>

> [!IMPORTANT]
> Clauds is an independent project. It is not made, endorsed, or supported by Anthropic.
> "Claude" is a trademark of Anthropic, PBC.

> [!WARNING]
> Early port. It builds and its tests pass, but it has not yet been tried against a real
> Claude Desktop for Linux. Make a backup before the first switch (see [Backup](#backup)).

## Requirements

- KDE Plasma 6 (Kubuntu 25.04 or later; built for 26.04). Other desktops work with the plain Qt tray.
- [Claude Desktop for Linux](https://claude.com/download) (`claude-desktop` package), or the unofficial
  `claude-desktop-unofficial` build. Both keep their data in `~/.config/Claude`.
- To build: CMake 3.22+, a C++20 compiler, Qt 6.4+ (Core, Widgets, Quick, WebEngine) and, for the
  native KDE integration, KDE Frameworks 6 (StatusNotifierItem, Notifications, CoreAddons, ColorScheme).

## Build and install on Kubuntu

```bash
sudo apt install cmake g++ extra-cmake-modules qt6-base-dev qt6-declarative-dev qt6-webengine-dev \
  libkf6statusnotifieritem-dev libkf6notifications-dev libkf6coreaddons-dev libkf6colorscheme-dev \
  qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-org-kde-desktop

git clone https://github.com/LemcheNET/clauds-kde.git
cd clauds-kde
cmake -S kde -B build-kde -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build-kde -j
ctest --test-dir build-kde            # 5 test suites
cd build-kde && cpack -G DEB && sudo apt install ./clauds-*.deb
```

Or use the `Makefile` in the repository root, which runs the same commands:

```bash
make deps      # install the build dependencies
make test      # configure, build, run the tests
make install   # build the .deb and install it
make help      # every target and setting
```

Then start **Clauds** from the application launcher. The icon appears in the system tray.

## Use

It works like the macOS app. See the main [README](../README.md#how-to-use-it) for the full
description. The differences:

| macOS | KDE Plasma |
| --- | --- |
| Menu bar icon | System tray icon. **Left click:** panel. **Right click:** quick menu with every account, for one-click switching. |
| Show Saved Accounts in Finder | Show Saved Accounts in File Manager (Dolphin) |
| Remove moves data to the Trash | Same: the freedesktop Trash, visible in Dolphin |
| Start at Login (Login Items) | An autostart entry, listed in System Settings > Autostart |
| Self-update (Sparkle) | No self-update. Install a new `.deb`. |
| Colors | Your color scheme (for example Quantum Light / Quantum Dark). Nothing is hard-coded. |

On Wayland, KWin decides where the panel opens. The quick menu (right click) always opens at the icon.

## Folders

| Path | What is in it |
| --- | --- |
| `~/.config/Claude` | Claude's data folder. Holds the login of the current account. |
| `~/.local/share/clauds/accounts.json` | The account list and settings. |
| `~/.local/share/clauds/Profiles/<id>` | The Claude data folder of each other account. |
| `~/.local/share/clauds/SharedSessions` | Shared Code session files, `owners.json`, `_conflicts`. |
| `~/.local/share/clauds/QtWebEngine/account-<id>` | The claude.ai login for live usage and chat windows. |

`~/.config` and `~/.local/share` must be on the same disk, because a switch renames folders.

## Settings for unusual installs

| Variable | Use |
| --- | --- |
| `CLAUDS_CLAUDE_LAUNCHER` | The command that starts Claude (default: `claude-desktop` on `PATH`). |
| `CLAUDS_CLAUDE_INSTALL_DIRS` | Colon-separated folders that hold the Claude executable, to find running copies. |
| `CLAUDS_CLAUDE_DATA_DIR` | Claude's data folder (default `~/.config/Claude`). |
| `CLAUDS_DATA_DIR` | Clauds' data folder (default `~/.local/share/clauds`). |

If Clauds does not see a running Claude, find its folder and set `CLAUDS_CLAUDE_INSTALL_DIRS`:

```bash
readlink -f "$(command -v claude-desktop)"
ps -eo pid,args | grep -i '[c]laude'
```

## Backup

```bash
tar -czf ~/claude-backup.tgz -C ~/.config Claude -C ~/.local/share clauds
```

## Development

- Design, platform mapping and open risks: [docs/DESIGN.md](docs/DESIGN.md).
- `src/core`: Qt Core only (account store, session layouts, usage, auto-switch, Claude process control).
- `src/app`: tray, panel (QML), live usage and chat windows (Qt WebEngine), KF6 integration.
- Without KF6 (`-DCLAUDS_WITH_KF6=OFF`), it builds a plain Qt tray app.
- Test hooks: `clauds --link-sessions <data dir> <shared dir> [tag]`, `--unlink-sessions …`,
  `--render-panel <png>` (with `QT_QPA_PLATFORM=offscreen`), `--show-panel`.

## Flatpak

The Flatpak (experimental) has the app ID `io.github.multiclauds.clauds-kde`. A release has it as
`io.github.multiclauds.clauds-kde-<version>.flatpak`. To build it yourself:

```bash
make flatpak-deps       # flatpak-builder, the KDE 6.10 runtime, SDK and Qt WebEngine base
make flatpak            # writes build-flatpak/io.github.multiclauds.clauds-kde.flatpak
make flatpak-install    # builds, then installs for your user
make flatpak-run
```

The sandbox opens the real `~/.config/Claude`, `~/.local/share/clauds`, the Trash and the autostart
folder, and it may talk to `org.freedesktop.Flatpak`. Claude Desktop is a host program, so the app
finds, quits and starts it with `flatpak-spawn --host`. Flatpak sets `XDG_DATA_HOME` to
`~/.var/app/<id>/data` whatever the manifest says, so the app uses `~/.local/share` and
`~/.config` of your home folder instead. Flatpak creates `~/.local/share/clauds`, the Trash and the
autostart folder on the host if they are missing, so the app sees the same folders as the host.
