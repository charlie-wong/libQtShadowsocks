#include <QDebug>
#include <QHostInfo>
#include <QTcpSocket>
#include <botan/init.h>

#include "controller.h"
#include "crypto/encryptor.h"

namespace QSS {

Controller::Controller(Profile profile, bool is_local,
    bool auto_ban, QObject *parent) :
    QObject(parent)
    , autoBan(auto_ban)
    , m_sent_bytes(0)
    , m_received_bytes(0)
    , m_profile(std::move(profile))
    , m_is_local(is_local)
{
    qInfo("Initialising cipher: %s", m_profile.method().data());

    // the default QHostAddress constructor will construct "::"
    // as AnyIPv6 we explicitly use Any to enable dual stack
    // which is the case in other shadowsocks ports
    if(m_profile.serverAddress() == "::") {
        m_server_addr = Address(QHostAddress::Any, m_profile.serverPort());
    } else {
        m_server_addr = Address(m_profile.serverAddress(), m_profile.serverPort());

        if(!m_server_addr.blockingLookUp()) {
            QDebug(QtMsgType::QtCriticalMsg).noquote().nospace()
                << "Cannot look up the host records of server address "
                << m_server_addr << ". Please make sure your Internet "
                << "connection is good and the configuration is correct";
        }
    }

    m_tcp_server = std::make_unique<QSS::TcpServer>(m_profile.method(),
        m_profile.password(), m_profile.timeout(),
        m_is_local, autoBan, m_server_addr
    );

    // FD_SETSIZE which is the maximum value on *nix platforms, 1024 by default
    m_tcp_server->setMaxPendingConnections(FD_SETSIZE);

    m_udp_relay = std::make_unique<QSS::UdpRelay>(m_profile.method(),
        m_profile.password(), m_is_local, autoBan, m_server_addr
    );

    connect(m_tcp_server.get(), &TcpServer::readBytes,
        this, &Controller::onReadBytes
    );
    connect(m_tcp_server.get(), &TcpServer::sendBytes,
        this, &Controller::onSendBytes
    );
    connect(m_tcp_server.get(), &TcpServer::acceptError,
        this, &Controller::onTcpServerError
    );
    connect(m_tcp_server.get(), &TcpServer::latencyAvailable,
        this, &Controller::onTcpLatencyAvailable
    );

    connect(m_udp_relay.get(), &UdpRelay::readBytes,
        this, &Controller::onReadBytes
    );
    connect(m_udp_relay.get(), &UdpRelay::sendBytes,
        this, &Controller::onSendBytes
    );
}

Controller::~Controller()
{
    if(m_tcp_server->isListening()) {
        stop();
    }
}

bool Controller::start()
{
    bool listen_ret = false;

    if(m_is_local) {
        qInfo("Running in client mode.");
        QHostAddress localAddr =
            m_profile.httpProxy() ? QHostAddress::LocalHost : getLocalAddr();
        listen_ret = m_tcp_server->listen(
            localAddr, m_profile.httpProxy() ? 0 : m_profile.localPort()
        );

        if(listen_ret) {
            listen_ret = m_udp_relay->listen(localAddr, m_profile.localPort());

            if(m_profile.httpProxy() && listen_ret) {
                QDebug(QtMsgType::QtInfoMsg).noquote()
                    << "SOCKS5 port is" << m_tcp_server->serverPort();
                m_http_proxy = std::make_unique<QSS::HttpProxy>();

                if(m_http_proxy->httpListen(getLocalAddr(),
                    m_profile.localPort(), m_tcp_server->serverPort())) {
                    qInfo("Running as a HTTP proxy server");
                } else {
                    listen_ret = false;
                    qCritical("HTTP proxy server listen failed.");
                }
            }
        }
    } else {
        qInfo("Running in server mode.");
        listen_ret = m_tcp_server->listen(m_server_addr.getFirstIP(),
            m_profile.serverPort()
        );

        if(listen_ret) {
            listen_ret = m_udp_relay->listen(m_server_addr.getFirstIP(),
                m_profile.serverPort()
            );
        }
    }

    if(listen_ret) {
        QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
            << "TCP server listening at "
            << (m_is_local ? getLocalAddr().toString()
               : m_server_addr.getFirstIP().toString())
            << ":"
            << (m_is_local ? m_profile.localPort() : m_profile.serverPort());
        emit runningStateChanged(true);
    } else {
        qCritical("TCP server listen failed.");
    }

    return listen_ret;
}

void Controller::stop()
{
    if(m_http_proxy) {
        m_http_proxy->close();
    }

    m_tcp_server->close();
    m_udp_relay->close();
    emit runningStateChanged(false);
    qInfo("Stopped.");
}

QHostAddress Controller::getLocalAddr()
{
    QHostAddress addr(QString::fromStdString(m_profile.localAddress()));

    if(!addr.isNull()) {
        return addr;
    }

    QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
        << "Can't get address from "
        << QString::fromStdString(m_profile.localAddress())
        << ". Using localhost instead.";
    return QHostAddress::LocalHost;
}

void Controller::onTcpServerError(QAbstractSocket::SocketError err)
{
    QDebug(QtMsgType::QtWarningMsg).noquote()
        << "TCP server error:" << m_tcp_server->errorString();

    // can't continue if address is already in use
    if(err == QAbstractSocket::AddressInUseError) {
        stop();
    }
}

void Controller::onReadBytes(quint64 r)
{
    // -1 means read failed, don't count
    if(r != static_cast<quint64>(-1)) {
        m_received_bytes += r;
        emit newBytesReceived(r);
        emit bytesReceivedChanged(m_received_bytes);
    }
}

void Controller::onSendBytes(quint64 s)
{
    // -1 means write failed, don't count
    if(s != static_cast<quint64>(-1)) {
        m_sent_bytes += s;
        emit newBytesSent(s);
        emit bytesSentChanged(m_sent_bytes);
    }
}

} // namespace QSS
