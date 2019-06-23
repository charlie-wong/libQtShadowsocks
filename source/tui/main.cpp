#include <signal.h>

#include <QCoreApplication>

#include "tui/tui.h"
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("ShadowSocks(TUI)");
    app.setApplicationVersion(QSS::Common::version());
    qInstallMessageHandler(Config::logMsgHandler);

    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);

    QtSsTui tui;
    CmdArgs opts;
    opts.process(app);

    // JSON configuration file
    QString configFile = opts.getConfigFile();
    if(configFile.isEmpty()) {
        qCritical() << "config.json missing, and can't not create!";
        return -1;
    }

    tui.applyConfigJson(configFile);

    // command-line option has higher priority
    if(!opts.getLogLevel().isEmpty()) {
        Config::setLogLevel(opts.getLogLevel());
    }

    if(opts.isSetServerMode()) {
        tui.setWorkMode(QSS::Profile::WorkMode::SERVER);
    }

    if(opts.isSetClientMode()) {
        tui.setWorkMode(QSS::Profile::WorkMode::CLIENT);
    }

    tui.setup(
        opts.getServerAddr(), opts.getServerPort(),
        opts.getProxyAddr(), opts.getProxyPort(),
        opts.getCryptoPassword(), opts.getCryptoAlgorithm(),
        opts.getSocketTimeout(), opts.isSetHttpProxy()
    );

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
