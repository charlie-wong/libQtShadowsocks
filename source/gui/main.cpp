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

    app.setApplicationVersion(SSQT_RELEASE_VERSION);
    app.setApplicationName(QString("ShadowSocks(GUI)"));
    app.setApplicationDisplayName(QString("ShadowSocksQt"));
    app.setWindowIcon(QIcon(":/icons/logo/shadowsocksqt.png"));

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

    QCommandLineParser cmdArgs;
    cmdArgs.addHelpOption();
    cmdArgs.addVersionOption();
    cmdArgs.process(app);
}

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<SQProfile>("SQProfile");
    QApplication app(argc, argv);
    setupApplication(app);

#ifdef Q_OS_WIN
    QString configFile = app.applicationDirPath() + "/ui.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/ShadowSocksQt";
    QString configFile = configDir.absolutePath() + "/ui.ini";

    if(!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }
#endif

    ConfigHelper config(configFile);
    MainWindow mainWindow(&config);

    if(config.isOnlyOneInstance() && mainWindow.isInstanceRunning()) {
        return -1;
    }

    mainWindow.startAutoStartConnections();

    if(!config.isHideWindowOnStartup()) {
        mainWindow.show();
    }

    return app.exec();
}
