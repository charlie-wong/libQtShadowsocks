#include <iostream>

#include <QDir>

#include "config/config.h"
#include "config/cmdArgs.h"
#include "config.generated.h"

CmdArgs::CmdArgs() :
    m_configFile("c",
        "specify config.json file.",
        "config_file"
    ),
    m_serverAddr("s",
        "host name or IP address for the remote server.",
        "server_addr"
    ),
    m_serverPort("p",
        "port number of the remote server.",
        "server_port"
    ),
    m_localAddr("b",
        "local proxy address to bind. ignored in server mode.",
        "proxy_addr",
        "127.0.0.1"
    ),
    m_localPort("n",
        "port number of the local proxy. ignored in server mode.",
        "proxy_port"
    ),
    m_cryptoPassword("k",
        "password for crypto algorithm.",
        "password"
    ),
    m_cryptoAlgorithm("m",
        "crypto algorithm method.",
        "algorithm"
    ),
    m_socketTimeout("t",
        "socket timeout in seconds.",
        "timeout"
    ),
    m_logLevel("L",
        "logging level: debug, info, warn, error, fatal.",
        "log_level",
        "info"
    ),
    m_serverMode(QStringList() << "S" << "server-mode",
        "run as shadowsocks server mode."
    ),
    m_clientMode(QStringList() << "C" << "client-mode",
        "run as shadowsocks client mode."
    ),
    m_httpProxy("http-proxy",
        "run in HTTP proxy mode. Ignored in server mode."
    ),
    m_testSpeed("speed-test",
        "test encrypt/decrypt speed."
    )
{
    m_parser.addOption(m_configFile);

    m_parser.addOption(m_serverAddr);
    m_parser.addOption(m_serverPort);

    m_parser.addOption(m_localAddr);
    m_parser.addOption(m_localPort);

    m_parser.addOption(m_cryptoPassword);
    m_parser.addOption(m_cryptoAlgorithm);

    m_parser.addOption(m_socketTimeout);
    m_parser.addOption(m_logLevel);

    m_parser.addOption(m_serverMode);
    m_parser.addOption(m_clientMode);
    m_parser.addOption(m_httpProxy);

    m_parser.addOption(m_testSpeed);

    m_parser.addHelpOption();
    m_parser.addVersionOption();
}

void CmdArgs::process(const QCoreApplication &app) {
    m_parser.process(app);
    Config::setLogLevel(m_parser.value(m_logLevel));
}

void CmdArgs::process(const QStringList &args) {
    m_parser.process(args);
    Config::setLogLevel(m_parser.value(m_logLevel));
}

QString CmdArgs::getConfigFile(void) const {
    // command line configuration come first
    QString configJson = m_parser.value(m_configFile);
    if(!configJson.isEmpty() && QFileInfo(configJson).isFile()) {
        return configJson;
    }

    // whether current working directory has 'config.json'
    configJson = QDir::currentPath() + QDir::separator() + "config.json";
    if(QFileInfo(configJson).isFile()) {
        return configJson;
    }

    // check the default 'config.json'
#ifdef Q_OS_WIN
    configJson = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir configDir = QDir::homePath() + "/.config/ShadowSocksQt";
    configJson = configDir.absolutePath() + "/config.json";

    if(!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }

    // create the default one if not exist, see DEFAULT_* macros
    if(!QFileInfo(configJson).isFile()) {
        QFile file(configJson);
        QString content(
            "{\n"
            "    \"mode\": \"client\",\n"
            "\n"
            "    \"server_port\": 12345,\n"
            "    \"server_addr\": \"ServerAddr\",\n"
            "\n"
            "    \"proxy_port\": 1080,\n"
            "    \"proxy_addr\": \"127.0.0.1\",\n"
            "\n"
            "    \"password\": \"ShadowSocks\",\n"
            "    \"algorithm\": \"aes-256-cfb\",\n"
            "\n"
            "    \"timeout\": 120,\n"
            "    \"http_proxy\": false,\n"
            "\n"
            "    \"log_level\": \"info\"\n"
            "}\n"
        );
        if(file.open(QIODevice::WriteOnly)) {
            file.write(content.toUtf8());
            file.close();
        }
    }
#endif

    return configJson;
}

QString CmdArgs::getServerAddr(void) const {
    return m_parser.value(m_serverAddr);
}

QString CmdArgs::getServerPort(void) const {
    return m_parser.value(m_serverPort);
}

QString CmdArgs::getProxyAddr(void) const {
    return m_parser.value(m_localAddr);
}

QString CmdArgs::getProxyPort(void) const {
    return m_parser.value(m_localPort);
}

QString CmdArgs::getCryptoPassword(void) const {
    return m_parser.value(m_cryptoPassword);
}

QString CmdArgs::getCryptoAlgorithm(void) const {
    return m_parser.value(m_cryptoAlgorithm);
}

QString CmdArgs::getSocketTimeout(void) const {
    return m_parser.value(m_socketTimeout);
}

QString CmdArgs::getLogLevel(void) const {
    return m_parser.value(m_logLevel);
}

bool CmdArgs::isSetServerMode(void) const {
    return m_parser.isSet(m_serverMode);
}

bool CmdArgs::isSetClientMode(void) const {
    return m_parser.isSet(m_clientMode);
}

bool CmdArgs::isSetHttpProxy(void) const {
    return m_parser.isSet(m_httpProxy);
}

bool CmdArgs::isSetSpeedTest(void) const {
    return m_parser.isSet(m_testSpeed);
}
