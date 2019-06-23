#include <QFile>
#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "config.generated.h"

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

    if(jsonObj.contains("Mode")) {
        QString mode = jsonObj["Mode"].toString();
        if(mode.compare("server", Qt::CaseInsensitive) == 0) {
            profile.setWorkMode(QSS::Profile::WorkMode::SERVER);
        } else if(mode.compare("client", Qt::CaseInsensitive) == 0) {
            profile.setWorkMode(QSS::Profile::WorkMode::CLIENT);
        }
    }

    if(jsonObj.contains("ClientPort")) {
        profile.setLocalPort((uint16_t)jsonObj["ClientPort"].toInt());
    }

    if(jsonObj.contains("ClientAddr")) {
        profile.setLocalAddress(jsonObj["ClientAddr"].toString().toStdString());
    }

    if(jsonObj.contains("ServerPort")) {
        profile.setServerPort((uint16_t)jsonObj["ServerPort"].toInt());
    }

    if(jsonObj.contains("ServerAddr")) {
        profile.setServerAddress(jsonObj["ServerAddr"].toString().toStdString());
    }

    if(jsonObj.contains("Algorithm")) {
        profile.setMethod(jsonObj["Algorithm"].toString().toStdString());
    }

    if(jsonObj.contains("Password")) {
        profile.setPassword(jsonObj["Password"].toString().toStdString());
    }

    if(jsonObj.contains("Timeout")) {
        profile.setSocketTimeout(jsonObj["Timeout"].toInt());
    }

    if(jsonObj.contains("HttpProxy")) {
        profile.setHttpProxy(jsonObj["HttpProxy"].toBool());
    }

    Config::setLogLevel(jsonObj["LogLevel"].toString());

    return true;
}

bool configJsonDefault(const QString &configJson, bool clientMode)
{
    QJsonObject jsonObj;

    if(clientMode) {
        jsonObj["Mode"] = "client";
    } else {
        jsonObj["Mode"] = "server";
    }

    jsonObj["LogLevel"] = "info";
    jsonObj["HttpProxy"] = "false";
    jsonObj["HttpsProxy"] = "false";

    jsonObj["Password"] = DEFAULT_PASSWORD;
    jsonObj["Algorithm"] = DEFAULT_ALGORITHM;

    jsonObj["ServerPort"] = DEFAULT_SERVER_PORT;
    jsonObj["ServerAddr"] = DEFAULT_SERVER_ADDR;
    jsonObj["ClientPort"] = DEFAULT_CLIENT_PORT;
    jsonObj["ClientAddr"] = DEFAULT_CLIENT_ADDR;

    jsonObj["Timeout"] = DEFAULT_SOCKET_TIMEOUT;

    QFile configFile(configJson);
    configFile.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!configFile.isOpen()) {
        qCritical() << "Error: cannot open " << configJson;
        return false;
    }

    if(!configFile.isWritable()) {
        qCritical() << "Error: cannot write into " << configJson;
        return false;
    }

    QJsonDocument jsonDoc(jsonObj);
    configFile.write(jsonDoc.toJson());
    configFile.close();

    return true;
}
