#include <signal.h>

#include <QDir>
#include <QCoreApplication>

#include "tui/tui.h"
#include "config/config.h"
#include "config/cmdArgs.h"

using namespace QSS;

static void onSignalRecv(int sig)
{
    if(sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    } else {
        qWarning("Unhandled Signal: %d", sig);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("ShadowSocks(TUI)");
    app.setApplicationVersion(QSS::Common::version());
    qInstallMessageHandler(Config::logMsgHandler);

    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);

    CmdArgs opts;
    opts.process(app);

    QtSsTui tui;

    QString configFile = opts.getConfigFile();
    if(configFile.isEmpty() || !QFileInfo(configFile).isFile()) {
        // whether application binary directory has 'config.json'
        QString appDir = QCoreApplication::applicationDirPath();
        configFile = appDir + QDir::separator() + "config.json";
    }

    if(!QFileInfo(configFile).isFile()) {
        // whether current working directory has 'config.json'
        configFile = QDir::currentPath() + QDir::separator() + "config.json";
    }

    if(!QFileInfo(configFile).isFile()) {
        qCritical() << "NOT found config.json, STOP!";
        return -1;
    }

    if(!tui.parseConfigJson(configFile)) {
        tui.setup(opts.getServerAddr(), opts.getServerPort(),
            opts.getProxyAddr(), opts.getProxyPort(),
            opts.getCryptoPassword(),
            opts.getCryptoAlgorithm(),
            opts.getSocketTimeout(),
            opts.isSetHttpProxy()
        );
    }

    // command-line option has a higher priority for S, C, H, T
    if(opts.isSetHttpProxy()) {
        tui.setHttpMode(true);
    }

    if(opts.isSetServerMode()) {
        tui.setWorkMode(QSS::Profile::WorkMode::SERVER);
    }

    if(opts.isSetClientMode()) {
        tui.setWorkMode(QSS::Profile::WorkMode::CLIENT);
    }

    if(opts.isSetSpeedTest()) {
        if(tui.getMethod().empty()) {
            qDebug() << "Testing all encryption methods ... ";
            Config::testSpeed(100);
        } else {
            Config::testSpeed(tui.getMethod(), 100);
        }

        return 0;
    } else if(tui.start()) {
        return app.exec();
    } else {
        return -1;
    }
}
