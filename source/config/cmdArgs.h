#ifndef CONFIG_CMDARGS_H
#define CONFIG_CMDARGS_H

#include <QCommandLineParser>

class CmdArgs {
public:
    CmdArgs();

    void process(const QCoreApplication &app);
    void process(const QStringList &arguments);

    bool isSetServerMode(void) const;
    bool isSetClientMode(void) const;
    bool isSetHttpProxy(void) const;
    bool isSetSpeedTest(void) const;

    QString getConfigFile(void) const;
    QString getServerAddr(void) const;
    QString getServerPort(void) const;
    QString getProxyAddr(void) const;
    QString getProxyPort(void) const;
    QString getCryptoPassword(void) const;
    QString getCryptoAlgorithm(void) const;
    QString getSocketTimeout(void) const;
    QString getLogLevel(void) const;

private:
    QCommandLineParser m_parser;

    QCommandLineOption m_configFile;
    QCommandLineOption m_serverAddr;
    QCommandLineOption m_serverPort;
    QCommandLineOption m_localAddr;
    QCommandLineOption m_localPort;
    QCommandLineOption m_cryptoPassword;
    QCommandLineOption m_cryptoAlgorithm;
    QCommandLineOption m_socketTimeout;
    QCommandLineOption m_logLevel;
    QCommandLineOption m_serverMode;
    QCommandLineOption m_clientMode;
    QCommandLineOption m_httpProxy;
    QCommandLineOption m_testSpeed;
};

#endif // CONFIG_CMDARGS_H
