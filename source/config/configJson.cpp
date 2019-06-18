#include <QFile>
#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "config/config.h"
#include "config/configJson.h"

bool configJsonApply(const QString &file, QSS::Profile &profile)
{
    QFile config(file);

    if(!config.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Can't open config file" << file;
        return false;
    }

    if(!config.isReadable()) {
        qWarning() << "Config file" << file << "is not readable!";
        return false;
    }

    QJsonParseError status;
    QJsonDocument confJson = QJsonDocument::fromJson(config.readAll(), &status);
    QJsonObject jsonObj = confJson.object();
    config.close();

    if(status.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config file" << status.errorString();
        return false;
    }

    if(jsonObj.contains("mode")) {
        QString mode = jsonObj["mode"].toString();
        if(mode.compare("server", Qt::CaseInsensitive) == 0) {
            profile.setWorkMode(QSS::Profile::WorkMode::SERVER);
        } else if(mode.compare("client", Qt::CaseInsensitive) == 0) {
            profile.setWorkMode(QSS::Profile::WorkMode::CLIENT);
        }
    }

    if(jsonObj.contains("proxy_port")) {
        profile.setLocalPort((uint16_t)jsonObj["proxy_port"].toInt());
    }

    if(jsonObj.contains("proxy_addr")) {
        profile.setLocalAddress(jsonObj["proxy_addr"].toString().toStdString());
    }

    if(jsonObj.contains("server_port")) {
        profile.setServerPort((uint16_t)jsonObj["server_port"].toInt());
    }

    if(jsonObj.contains("server_addr")) {
        profile.setServerAddress(jsonObj["server_addr"].toString().toStdString());
    }

    if(jsonObj.contains("algorithm")) {
        profile.setMethod(jsonObj["algorithm"].toString().toStdString());
    }

    if(jsonObj.contains("password")) {
        profile.setPassword(jsonObj["password"].toString().toStdString());
    }

    if(jsonObj.contains("timeout")) {
        profile.setSocketTimeout(jsonObj["timeout"].toInt());
    }

    if(jsonObj.contains("http_proxy")) {
        profile.setHttpProxy(jsonObj["http_proxy"].toBool());
    }

    Config::setLogLevel(jsonObj["log_level"].toString());

    return true;
}
