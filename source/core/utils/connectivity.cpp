// Simple way to test the connection's latency. Since it's a
// just socket connection without any data transfer, the remote
// doesn't need to be a shadowsocks server.

#include "common.h"
#include "connectivity.h"
#include "network/address.h"
#include "crypto/encryptor.h"

namespace QSS {

Connectivity::Connectivity(const QHostAddress &server_addr,
    const uint16_t &server_port, QObject *parent) :
    QObject(parent)
    , m_server_addr(server_addr)
    , m_server_port(server_port)
    , m_do_connectivity_test(false)
{
    m_timestamp = QTime::currentTime();

    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &Connectivity::onTimeout);

    m_socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(&m_socket, &QTcpSocket::connected,
        this, &Connectivity::onConnected
    );
    connect(&m_socket, &QTcpSocket::readyRead,
        this, &Connectivity::onSocketReadyRead
    );
    connect(&m_socket,
        static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
        (&QTcpSocket::error), this, &Connectivity::onSocketError
    );
}

void Connectivity::connectToServer(int timeout)
{
    m_timestamp = QTime::currentTime();
    m_timer.start(timeout);
    m_socket.connectToHost(m_server_addr, m_server_port);
}

void Connectivity::lagTestStart(int timeout)
{
    m_do_connectivity_test = false;
    connectToServer(timeout);
}

void Connectivity::connTestStart(const std::string &method,
    const std::string &password, int timeout)
{
    m_do_connectivity_test = true;
    encryptionMethod = method;
    encryptionPassword = password;
    connectToServer(timeout);
}

void Connectivity::onTimeout()
{
    m_socket.abort();
    emit connTestFinished(false);
    emit lagTestFinished(LAG_TIMEOUT);
}

void Connectivity::onSocketError(QAbstractSocket::SocketError)
{
    m_timer.stop();
    m_socket.abort();
    emit connTestFinished(false);
    emit testResult(m_socket.errorString());
    emit lagTestFinished(LAG_ERROR);
}

void Connectivity::onConnected()
{
    m_timer.stop();
    emit lagTestFinished(m_timestamp.msecsTo(QTime::currentTime()));

    if(m_do_connectivity_test) {
        /// @todo: find a better way to check connectivity
        // A http request to Google to test the connectivity.
        // The payload is dumped from
        // `curl http://www.google.com --socks5 127.0.0.1:1080`
        Encryptor encryptor(encryptionMethod, encryptionPassword);
        std::string dest =
            Common::packAddress(Address("www.google.com", 80));
        static const QByteArray expected = QByteArray::fromHex(
            "474554202f20485454502f312e310d0a486f73743a"
            "207777772e676f6f676c652e636f6d0d0a55736572"
            "2d4167656e743a206375726c2f372e34332e300d0a"
            "4163636570743a202a2f2a0d0a0d0a"
        );
        std::string payload(expected.data(), expected.length());
        std::string toWrite = encryptor.encrypt(dest + payload);
        m_socket.write(toWrite.data(), toWrite.size());
    } else {
        m_socket.abort();
    }
}

void Connectivity::onSocketReadyRead()
{
    emit connTestFinished(true);
    m_socket.abort();
}

} // namespace QSS
