#include <QFile>
#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "config/configJson.h"

bool configJsonParser(const QString &file, QSS::Profile &profile)
{
    QFile config(file);

    if(!config.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QDebug(QtMsgType::QtCriticalMsg).noquote()
            << "Can't open configuration file" << file;
        return false;
    }

    if(!config.isReadable()) {
        QDebug(QtMsgType::QtCriticalMsg).noquote()
            << "Configuration file" << file << "is not readable!";
        return false;
    }

    QJsonParseError status;
    QJsonDocument confJson = QJsonDocument::fromJson(config.readAll(), &status);
    QJsonObject jsonObj = confJson.object();
    config.close();

    if(status.error != QJsonParseError::NoError) {
        qCritical() << "Failed to parse configuration file:" << status.errorString();
        return false;
    }

    QString work_mode = jsonObj["mode"].toString();
    if(work_mode.compare("server", Qt::CaseInsensitive) == 0) {
        profile.setWorkMode(QSS::Profile::WorkMode::SERVER);
    } else if(work_mode.compare("client", Qt::CaseInsensitive) == 0) {
        profile.setWorkMode(QSS::Profile::WorkMode::CLIENT);
    }

    profile.setLocalPort((uint16_t)jsonObj["proxy_port"].toInt());
    profile.setLocalAddress(jsonObj["proxy_addr"].toString().toStdString());

    profile.setServerPort((uint16_t)jsonObj["server_port"].toInt());
    profile.setServerAddress(jsonObj["server_addr"].toString().toStdString());

    profile.setMethod(jsonObj["algorithm"].toString().toStdString());
    profile.setPassword(jsonObj["password"].toString().toStdString());

    profile.setTimeout(jsonObj["timeout"].toInt());
    profile.setHttpProxy(jsonObj["http_proxy"].toBool());

    return true;
}
