#include <signal.h>
#include <iostream>

#include <QDir>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "tui.h"
#include "utils.h"

using namespace QSS;

static void on_SIGINT_SIGTERM(int sig)
{
    if(sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    }
}

Utils::LogLevel stringToLogLevel(const QString &str)
{
    if(str.compare("DEBUG", Qt::CaseInsensitive) == 0) {
        return Utils::LogLevel::DEBUG;
    } else if(str.compare("INFO", Qt::CaseInsensitive) == 0) {
        return Utils::LogLevel::INFO;
    } else if(str.compare("WARN", Qt::CaseInsensitive) == 0) {
        return Utils::LogLevel::WARN;
    } else if(str.compare("ERROR", Qt::CaseInsensitive) == 0) {
        return Utils::LogLevel::ERROR;
    } else if(str.compare("FATAL", Qt::CaseInsensitive) == 0) {
        return Utils::LogLevel::FATAL;
    }

    std::cerr << "Log level [" << str.toStdString()
        << "] is not recognised, default to INFO" << std::endl;
    return Utils::LogLevel::INFO;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("ShadowSocks(TUI)");
    app.setApplicationVersion(QSS::Common::version());
    qInstallMessageHandler(Utils::messageHandler);

    signal(SIGINT, on_SIGINT_SIGTERM);
    signal(SIGTERM, on_SIGINT_SIGTERM);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption configFile("c",
        "specify config.json file.",
        "config_file"
    );
    QCommandLineOption serverAddress("s",
        "host name or IP address for the remote server.",
        "server_address"
    );
    QCommandLineOption serverPort("p",
        "port number of the remote server.",
        "server_port"
    );
    QCommandLineOption localAddress("b",
        "local address to bind. ignored in server mode.",
        "local_address",
        "127.0.0.1"
    );
    QCommandLineOption localPort("l",
        "port number of the local server. ignored in server mode.",
        "local_port"
    );
    QCommandLineOption password("k",
        "password of the remote server.",
        "password"
    );
    QCommandLineOption encryptionMethod("m",
        "encryption method.",
        "method"
    );
    QCommandLineOption timeout("t",
        "socket timeout in seconds.",
        "timeout"
    );
    QCommandLineOption serverMode(
        QStringList() << "S" << "server-mode",
        "run as shadowsocks server."
    );
    QCommandLineOption clientMode(
        QStringList() << "C" << "client-mode",
        "run as shadowsocks client."
    );
    QCommandLineOption http(
        QStringList() << "H" << "http-proxy",
        "run in HTTP(S) proxy mode. ignored in server mode."
    );
    QCommandLineOption testSpeed(
        QStringList() << "T" << "speed-test",
        "test encrypt/decrypt speed."
    );
    QCommandLineOption log("L",
        "log level: debug, info, warn, error, fatal.",
        "log_level",
        "info"
    );
    QCommandLineOption autoBan("autoban",
        "automatically ban IPs that send malformed header. "
        "ignored in local mode."
    );
    parser.addOption(configFile);
    parser.addOption(serverAddress);
    parser.addOption(serverPort);
    parser.addOption(localAddress);
    parser.addOption(localPort);
    parser.addOption(password);
    parser.addOption(encryptionMethod);
    parser.addOption(timeout);
    parser.addOption(http);
    parser.addOption(serverMode);
    parser.addOption(clientMode);
    parser.addOption(testSpeed);
    parser.addOption(log);
    parser.addOption(autoBan);
    parser.process(app);

    QtSsTui c;
    Utils::logLevel = stringToLogLevel(parser.value(log));

    QString jsonConfig = parser.value(configFile);
    if(jsonConfig.isEmpty()) {
        jsonConfig = QCoreApplication::applicationDirPath()
          + QDir::separator() + "config.json";
    }

    if(!c.readConfig(jsonConfig)) {
        c.setup(parser.value(serverAddress),
            parser.value(serverPort),
            parser.value(localAddress),
            parser.value(localPort),
            parser.value(password),
            parser.value(encryptionMethod),
            parser.value(timeout),
            parser.isSet(http)
        );
    }

    c.setAutoBan(parser.isSet(autoBan));

    // command-line option has a higher priority for S, C, H, T
    if(parser.isSet(http)) {
        c.setHttpMode(true);
    }

    if(parser.isSet(serverMode)) {
        c.setWorkMode(QtSsTui::WorkMode::SERVER);
    }

    if(parser.isSet(clientMode)) {
        c.setWorkMode(QtSsTui::WorkMode::CLIENT);
    }

    if(parser.isSet(testSpeed)) {
        if(c.getMethod().empty()) {
            std::cout << "Testing all encryption methods ... " << std::endl;
            Utils::testSpeed(100);
        } else {
            Utils::testSpeed(c.getMethod(), 100);
        }

        return 0;
    } else if(c.start()) {
        return app.exec();
    } else {
        return -1;
    }
}
