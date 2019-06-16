#include <QFile>
#include <QDebug>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

#include "tui.h"

QtSsTui::QtSsTui() : autoBan(false), m_work_mode(CLIENT)
{
    // Nothing Todo
}

bool QtSsTui::readConfig(const QString &file)
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
        setWorkMode(SERVER);
    } else if(work_mode.compare("client", Qt::CaseInsensitive) == 0) {
        setWorkMode(CLIENT);
    }

    m_profile.setLocalAddress(jsonObj["local"].toString().toStdString());
    m_profile.setLocalPort((uint16_t)jsonObj["local_port"].toInt());

    m_profile.setServerAddress(jsonObj["server"].toString().toStdString());
    m_profile.setServerPort((uint16_t)jsonObj["server_port"].toInt());

    m_profile.setMethod(jsonObj["method"].toString().toStdString());
    m_profile.setPassword(jsonObj["password"].toString().toStdString());

    m_profile.setTimeout(jsonObj["timeout"].toInt());
    m_profile.setHttpProxy(jsonObj["http_proxy"].toBool());

    if(jsonObj["auth"].toBool()) {
        QDebug(QtMsgType::QtCriticalMsg)
            << "OTA is deprecated, remove OTA from the configuration file.";
    }

    return true;
}

void QtSsTui::setup(const QString &remote_addr, const QString &remote_port,
    const QString &local_addr, const QString &local_port,
    const QString &password, const QString &method,
    const QString &timeout, const bool http_proxy)
{
    m_profile.setServerAddress(remote_addr.toStdString());
    m_profile.setServerPort((uint16_t)remote_port.toInt());
    m_profile.setLocalAddress(local_addr.toStdString());
    m_profile.setLocalPort((uint16_t)local_port.toInt());
    m_profile.setPassword(password.toStdString());
    m_profile.setMethod(method.toStdString());
    m_profile.setTimeout(timeout.toInt());
    m_profile.setHttpProxy(http_proxy);
}

void QtSsTui::setAutoBan(bool ban)
{
    autoBan = ban;
}

void QtSsTui::setHttpMode(bool http)
{
    m_profile.setHttpProxy(http);
}

void QtSsTui::setWorkMode(WorkMode mode) {
    m_work_mode = mode;
}

bool QtSsTui::start(void)
{
    if(m_profile.debug()) {
        if(!headerTest()) {
            QDebug(QtMsgType::QtCriticalMsg) << "Header test failed.";
            return false;
        }
    }

    if(!m_profile.isValid()) {
        qCritical() << "The m_profile is invalid, check it and try again!";
        return false;
    }

    bool server_mode = false;
    if(SERVER == m_work_mode) {
        server_mode = true;
    }

    m_ctrl.reset(new QSS::Controller(m_profile, !server_mode, autoBan));

    if(!server_mode) {
        QSS::Address server(m_profile.serverAddress(), m_profile.serverPort());
        server.blockingLookUp();
        m_conn.reset(new QSS::Connectivity(server.getFirstIP(),
            server.getPort())
        );

        QObject::connect(m_conn.get(), &QSS::Connectivity::connectivityFinished,
        [](bool c) {
            if(c) {
                QDebug(QtMsgType::QtInfoMsg) << "The shadowsocks connection is okay.";
            } else {
                QDebug(QtMsgType::QtWarningMsg)
                    << "Destination is not reachable. "
                       "Please check your network and firewall settings.";
                QCoreApplication::exit(-1);
            }
        });

        QObject::connect(m_conn.get(), &QSS::Connectivity::testErrorString,
        [](const QString & error) {
            QDebug(QtMsgType::QtWarningMsg).noquote()
                << "Connectivity testing error:" << error;
        });

        m_conn->startConnectivityTest(m_profile.method(), m_profile.password());
    }

    return m_ctrl->start();
}

bool QtSsTui::headerTest()
{
    int length;
    QHostAddress test_addr("1.2.3.4");
    QHostAddress test_addr_v6("2001:0db8:85a3:0000:0000:8a2e:1010:2020");
    uint16_t test_port = 56;
    QSS::Address test_res, test_v6(test_addr_v6, test_port);
    std::string packed = QSS::Common::packAddress(test_v6);
    QSS::Common::parseHeader(packed, test_res, length);
    bool success = (test_v6 == test_res);

    if(!success) {
        qWarning("%s --> %s",
            test_v6.toString().data(), test_res.toString().data()
        );
    }

    packed = QSS::Common::packAddress(test_addr, test_port);
    QSS::Common::parseHeader(packed, test_res, length);

    bool success2 = ((test_res.getFirstIP() == test_addr)
        && (test_res.getPort() == test_port)
    );

    if(!success2) {
        QDebug(QtMsgType::QtWarningMsg).noquote().nospace()
            << test_addr.toString() << ":" << test_port
            << " --> " << test_res.toString().data();
    }

    return success & success2;
}

const std::string &QtSsTui::getMethod() const
{
    return m_profile.method();
}
