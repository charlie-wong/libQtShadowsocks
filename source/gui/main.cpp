#include <signal.h>

#include <QDir>
#include <QDebug>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QApplication>
#include <QCommandLineParser>

#include "mainwindow.h"
#include "confighelper.h"
#include "config/config.h"
#include "config/cmdArgs.h"

MainWindow *mainWindow = nullptr;

static void onSignalRecv(int sig)
{
    if(sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    } else {
        qWarning("Unhandled Signal: %d", sig);
    }
}

static void setupApplication(QApplication &app)
{
    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);
    QApplication::setFallbackSessionManagementEnabled(false);
    app.setApplicationName(QString("ShadowSocks(GUI)"));
    app.setApplicationDisplayName(QString("ShadowSocksQt"));
    app.setApplicationVersion(SSQT_RELEASE_VERSION);

#ifdef Q_OS_WIN
    if(QLocale::system().country() == QLocale::China) {
        app.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
    } else {
        app.setFont(QFont("Segoe UI", 9, QFont::Normal, false));
    }
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QIcon::setThemeName("Breeze");
#endif

    QTranslator *trans = new QTranslator(&app);
    trans->load(QLocale::system(), "ssqt", "_", ":/i18n");
    app.installTranslator(trans);
}

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<SQProfile>("SQProfile");
    QApplication app(argc, argv);
    setupApplication(app);
    CmdArgs opts;
    opts.process(app);
    QString configFile = "";

    if(configFile.isEmpty()) {
    #ifdef Q_OS_WIN
        configFile = app.applicationDirPath() + "/config.ini";
    #else
        QDir configDir = QDir::homePath() + "/.config/ShadowSocksQt";
        configFile = configDir.absolutePath() + "/config.ini";

        if(!configDir.exists()) {
            configDir.mkpath(configDir.absolutePath());
        }
    #endif
    }

    ConfigHelper conf(configFile);
    MainWindow w(&conf);
    mainWindow = &w;

    if(conf.isOnlyOneInstance() && w.isInstanceRunning()) {
        return -1;
    }

    w.startAutoStartConnections();

    if(!conf.isHideWindowOnStartup()) {
        w.show();
    }

    return app.exec();
}
