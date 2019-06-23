#include <iostream>

#include <QDir>

#include "config.generated.h"

#include "config/config.h"
#include "config/cmdArgs.h"
#include "config/configJson.h"

CmdArgs::CmdArgs() :
    m_configFile("c",
        "specify config.json file.",
        "ConfigJson"
    ),
    m_serverAddr("s",
        "host name or IP address for the remote server.",
        "ServerAddr"
    ),
    m_serverPort("p",
        "port number of the remote server.",
        "ServerPort"
    ),
    m_localAddr("b",
        "local client address to bind. ignored in server mode.",
        "ClientAddr",
        "127.0.0.1"
    ),
    m_localPort("n",
        "port number of the local client. ignored in server mode.",
        "ClientPort"
    ),
    m_cryptoPassword("k",
        "password for crypto algorithm.",
        "Password"
    ),
    m_cryptoAlgorithm("m",
        "crypto algorithm method.",
        "Algorithm"
    ),
    m_socketTimeout("t",
        "socket timeout in seconds.",
        "Timeout"
    ),
    m_logLevel("L",
        "log level: debug, info, warn, error, fatal.",
        "LogLevel",
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
#endif

    // create the default one if not exist, see DEFAULT_* macros
    if(!QFileInfo(configJson).isFile()) {
        if(!configJsonDefault(configJson, false)) {
            return ""; // can't create the default config.json
        }
    }

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
