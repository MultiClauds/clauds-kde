// SPDX-License-Identifier: MIT
// Clauds for KDE Plasma: switches the Claude desktop app between accounts.
#include "AccountStore.h"
#include "Actions.h"
#include "ChatWindows.h"
#include "LiveUsage.h"
#include "LoginItem.h"
#include "SessionLinker.h"
#include "Theme.h"
#include "Tray.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickItem>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#ifdef CLAUDS_HAVE_KF6
#include <KAboutData>
#endif

#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>

using namespace clauds;

namespace {

/// Test hooks, same as the macOS app: clauds --link-sessions|--unlink-sessions <data dir> <shared dir> [tag]
int runLinkerHook(const QStringList &args)
{
    const bool link = args.at(1) == QLatin1String("--link-sessions");
    std::optional<QString> tag;
    if (args.size() > 4)
        tag = args.at(4);
    try {
        const std::filesystem::path data(args.at(2).toStdString()), shared(args.at(3).toStdString());
        const int n = link ? SessionLinker::link(data, shared, tag) : SessionLinker::unlink(data, shared, tag);
        std::printf("%s %d folder(s)\n", link ? "linked" : "unlinked", n);
        return 0;
    } catch (const std::exception &e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }
}

QString socketName() { return QStringLiteral("clauds-%1").arg(::getuid()); }

/// Web data of accounts that were removed. Their profiles are no longer loaded, so this is safe.
void removeStaleProfiles(const AccountStore &store)
{
    // QWebEngineProfile keeps named profiles in <AppDataLocation>/QtWebEngine/<name>.
    QDir root(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/QtWebEngine"));
    QSet<QString> keep;
    for (const auto &a : store.accounts())
        keep.insert(QStringLiteral("account-") + a.id);
    for (const auto &name : root.entryList({QStringLiteral("account-*")}, QDir::Dirs))
        if (!keep.contains(name))
            QDir(root.filePath(name)).removeRecursively();
}

} // namespace

int main(int argc, char **argv)
{
    // Every file and folder that this app makes is for this user only.
    ::umask(077);

    // Flatpak sets XDG_DATA_HOME to ~/.var/app/<id>/data whatever the manifest says. This app
    // works on the real data folder (~/.local/share/clauds) and on the real Trash, which the
    // manifest opens. This has to be set before Qt reads the folder.
    if (ClaudeProcess::inFlatpak() && !qEnvironmentVariableIsSet("CLAUDS_DATA_DIR"))
        qputenv("XDG_DATA_HOME", (QDir::homePath() + QStringLiteral("/.local/share")).toLocal8Bit());

    {
        QStringList args;
        for (int i = 0; i < argc; ++i)
            args << QString::fromLocal8Bit(argv[i]);
        if (args.size() >= 4 && (args.at(1) == QLatin1String("--link-sessions") || args.at(1) == QLatin1String("--unlink-sessions"))) {
            QCoreApplication core(argc, argv);
            return runLinkerHook(args);
        }
    }

    // Qt caches the compiled QML on disk and checks it against the modification time of the file.
    // The files inside the binary have a fixed time in a Flatpak build, so after an update the old
    // cached panel kept being shown. The QML is small; compile it at every start.
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // In the Flatpak the KDE Controls style (qqc2-desktop-style) takes its text colors from
    // kdeglobals, which is stale there after a change of the color scheme. Fusion draws from the
    // palette, which follows the scheme (see Theme.cpp).
    if (QFile::exists(QStringLiteral("/.flatpak-info")) && !qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_STYLE"))
        qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("clauds"));
    app.setOrganizationName(QStringLiteral("clauds"));
    app.setApplicationDisplayName(QStringLiteral("Clauds"));
    app.setApplicationVersion(QStringLiteral(CLAUDS_VERSION));
    app.setDesktopFileName(QStringLiteral("io.github.multiclauds.clauds-kde"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("clauds"), QIcon(QStringLiteral(":/icons/clauds.svg"))));
    app.setQuitOnLastWindowClosed(false);

#ifdef CLAUDS_HAVE_KF6
    KAboutData about(QStringLiteral("clauds"), QStringLiteral("Clauds"), QStringLiteral(CLAUDS_VERSION),
                     QStringLiteral("Switches the Claude desktop app between accounts"), KAboutLicense::MIT,
                     QStringLiteral("© 2026 Razvan Tanase (Clauds), KDE port contributors"));
    about.setHomepage(QStringLiteral("https://products.lemche.net/clauds-kde/"));
    about.setDesktopFileName(QStringLiteral("io.github.multiclauds.clauds-kde"));
    KAboutData::setApplicationData(about);
#endif

    // Build check / docs: clauds --render-panel <png>. Draws the panel with the accounts in
    // CLAUDS_DATA_DIR, without live usage, and quits. Use QT_QPA_PLATFORM=offscreen.
    if (const int at = app.arguments().indexOf(QStringLiteral("--render-panel")); at > 0 && at + 1 < app.arguments().size()) {
        AccountStore store(AccountStore::Paths::standard(), ClaudeProcess::discover());
        store.blockSignals(true);   // no live usage requests
        store.refreshUsage();
        LiveUsage live;
        ChatWindows chats(&live);
        Actions actions(&store, &live, &chats);
        Theme theme;
        QQuickView view;
        view.setResizeMode(QQuickView::SizeViewToRootObject);
        view.rootContext()->setContextProperty(QStringLiteral("store"), &store);
        view.rootContext()->setContextProperty(QStringLiteral("actions"), &actions);
        view.rootContext()->setContextProperty(QStringLiteral("theme"), &theme);
        view.setSource(QUrl(QStringLiteral("qrc:/qml/Panel.qml")));
        if (view.status() != QQuickView::Ready)
            return 1;
        view.show();
        QTimer::singleShot(300, &app, [&] {
            const bool ok = view.grabWindow().save(app.arguments().at(at + 1));
            QCoreApplication::exit(ok ? 0 : 1);
        });
        return app.exec();
    }

    // Only one Clauds can run: two at the same time could move the same files.
    // A second start (or "clauds --show-panel") asks the running one to open its panel.
    {
        QLocalSocket probe;
        probe.connectToServer(socketName());
        if (probe.waitForConnected(500)) {
            probe.write("show\n");
            probe.waitForBytesWritten(500);
            return 0;
        }
        if (app.arguments().contains(QStringLiteral("--show-panel"))) {
            std::fprintf(stderr, "Clauds is not running.\n");
            return 1;
        }
    }
    QLocalServer::removeServer(socketName());
    QLocalServer server;
    server.setSocketOptions(QLocalServer::UserAccessOption);
    server.listen(socketName());

    AccountStore store(AccountStore::Paths::standard(), ClaudeProcess::discover());
    removeStaleProfiles(store);
    LiveUsage live;
    ChatWindows chats(&live);
    Actions actions(&store, &live, &chats);
    Theme theme;
    Tray tray(&store, &actions, &theme);

    QObject::connect(&server, &QLocalServer::newConnection, &tray, [&server, &tray] {
        while (auto *socket = server.nextPendingConnection()) {
            QObject::connect(socket, &QLocalSocket::readyRead, &tray, [socket, &tray] {
                if (socket->readAll().startsWith("show"))
                    tray.showPanel();
                socket->deleteLater();
            });
        }
    });

    if (!store.claudeInstalled()) {
        QTimer::singleShot(0, [] {
            QMessageBox::information(nullptr, QStringLiteral("Clauds"),
                                     QStringLiteral("The Claude desktop app was not found.\n\nInstall the claude-desktop "
                                                    "package from claude.com/download, or set CLAUDS_CLAUDE_LAUNCHER to "
                                                    "the command that starts Claude."));
        });
    }

    // First start: ask one time about Start at Login. Never turn it on without a yes.
    QSettings settings;
    if (!settings.value(QStringLiteral("askedLoginItem")).toBool()) {
        settings.setValue(QStringLiteral("askedLoginItem"), true);
        QTimer::singleShot(500, &actions, [&actions] {
            if (QMessageBox::question(nullptr, QStringLiteral("Start Clauds at login?"),
                                      QStringLiteral("Clauds can start when you log in, so the account switcher is "
                                                     "always in the system tray. You can change this later in the … menu.")) ==
                QMessageBox::Yes)
                actions.setStartsAtLogin(true);
        });
    }

    return app.exec();
}
