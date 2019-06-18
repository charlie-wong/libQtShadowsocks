#include <QFile>
#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "tui.h"
#include "config/configJson.h"

QtSsTui::QtSsTui()
{
    // Nothing Todo
}

bool QtSsTui::parseConfigJson(const QString &file)
{
    return configJsonParser(file, m_profile);
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

void QtSsTui::setHttpMode(bool http)
{
    m_profile.setHttpProxy(http);
}

void QtSsTui::setWorkMode(QSS::Profile::WorkMode mode) {
    m_profile.setWorkMode(mode);
}

bool QtSsTui::start(void)
{
    if(m_profile.debug()) {
        if(!headerTest()) {
            qCritical() << "Header test failed.";
            return false;
        }
    }

    if(!m_profile.isValid()) {
        qCritical() << "The m_profile is invalid, check it and try again!";
        return false;
    }

    bool server_mode = false;
    if(QSS::Profile::WorkMode::SERVER == m_profile.getWorkMode()) {
        server_mode = true;
    }

    m_ctrl.reset(new QSS::Controller(m_profile, !server_mode, false));

    if(!server_mode) {
        QSS::Address server(m_profile.serverAddress(), m_profile.serverPort());
        server.blockingLookUp();
        m_conn.reset(
            new QSS::Connectivity(server.getFirstIP(), server.getPort())
        );

        QObject::connect(m_conn.get(), &QSS::Connectivity::connTestFinished,
        [](bool c) {
            if(c) {
                qInfo() << "The shadowsocks connection is okay.";
            } else {
                qWarning() << "Destination is not reachable. "
                    "Please check your network and firewall settings.";
            }
        });

        QObject::connect(m_conn.get(), &QSS::Connectivity::testResult,
        [](const QString & error) {
            qWarning() << "Connectivity testing error:" << error;
        });

        m_conn->connTestStart(m_profile.method(), m_profile.password());
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
        qWarning() << test_addr.toString() << ":" << test_port
            << "-->" << test_res.toString().data();
    }

    return success & success2;
}

const std::string &QtSsTui::getMethod() const
{
    return m_profile.method();
}
