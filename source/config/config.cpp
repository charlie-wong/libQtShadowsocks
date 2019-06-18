#include <iostream>

#include <QTime>
#include <QDateTime>

#include <QtShadowsocks>

#include "config/config.h"

static Config::LogLevel log_level = Config::LogLevel::WARN;

static Config::LogLevel logLevelConv(const QString &str)
{
    if(str.compare("DEBUG", Qt::CaseInsensitive) == 0) {
        return Config::LogLevel::DEBUG;
    } else if(str.compare("INFO", Qt::CaseInsensitive) == 0) {
        return Config::LogLevel::INFO;
    } else if(str.compare("WARN", Qt::CaseInsensitive) == 0) {
        return Config::LogLevel::WARN;
    } else if(str.compare("ERROR", Qt::CaseInsensitive) == 0) {
        return Config::LogLevel::ERROR;
    } else if(str.compare("FATAL", Qt::CaseInsensitive) == 0) {
        return Config::LogLevel::FATAL;
    }

    return Config::LogLevel::WARN; // default log level
}

void Config::setLogLevel(const QString &logLevelName) {
    log_level = logLevelConv(logLevelName);
}

void Config::testSpeed(const std::string &method, uint32_t data_size_mb)
{
    const std::string test(1024 * 32, '#'); // 32KB
    uint32_t loops = 32 * data_size_mb;
    QSS::Encryptor enc(method, "barfoo!");
    QTime startTime = QTime::currentTime();

    // encrypt per 1MB to reduce memory usage during the test
    for(uint32_t i = 0; i < loops; ++i) {
        enc.encrypt(test);
    }

    std::cout << "Encrypt Method      : " << method << "\n"
        << "Datagram size       : " << data_size_mb << "MB\n"
        << "Time used to encrypt: "
        << startTime.msecsTo(QTime::currentTime()) << "ms\n" << std::endl;
}

void Config::testSpeed(uint32_t data_size_mb)
{
    std::vector<std::string> allMethods = QSS::Cipher::supportedMethods();
    std::sort(allMethods.begin(), allMethods.end());

    for(const auto &method : allMethods) {
        testSpeed(method, data_size_mb);
    }
}

void Config::logMsgHandler(QtMsgType type,
    const QMessageLogContext &, const QString &msg)
{
    const std::string message = msg.toStdString();
    const QDateTime now = QDateTime::currentDateTime();
    const std::string timestamp =
        now.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();

    switch(type) {
        case QtDebugMsg: // qDebug()
            if(log_level <= LogLevel::DEBUG) {
                std::cout << timestamp << " DEBUG: " << message << std::endl;
            }

            break;

        case QtInfoMsg: // qInfo()
            if(log_level <= LogLevel::INFO) {
                std::cout << timestamp << " INFO: " << message << std::endl;
            }

            break;

        case QtWarningMsg: // qWarning()
            if(log_level <= LogLevel::WARN) {
                std::cerr << timestamp << " WARN: " << message << std::endl;
            }

            break;

        case QtCriticalMsg: // qCritical()
            if(log_level <= LogLevel::ERROR) {
                std::cerr << timestamp << " ERROR: " << message << std::endl;
            }

            break;

        case QtFatalMsg: // qFatal()
            std::cerr << timestamp << " FATAL: " << message << std::endl;
            abort(); // fatal error, abort
    }
}
