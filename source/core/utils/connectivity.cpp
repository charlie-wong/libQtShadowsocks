// Simple way to test the connection's latency. Since it's a
// just socket connection without any data transfer, the remote
// doesn't need to be a shadowsocks server.

#include "common.h"
#include "connectivity.h"
#include "network/address.h"
#include "crypto/encryptor.h"

namespace QSS {

Connectivity::Connectivity(const QHostAddress &_address,
    const uint16_t &_port, QObject *parent) :
    QObject(parent)
    , address(_address)
    , port(_port)
    , testingConnectivity(false)
{
    timer.setSingleShot(true);
    time = QTime::currentTime();
    socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(&timer, &QTimer::timeout, this, &Connectivity::onTimeout);
    connect(&socket, &QTcpSocket::connected, this, &Connectivity::onConnected);
    connect(&socket, &QTcpSocket::readyRead,
        this, &Connectivity::onSocketReadyRead
    );
    connect(&socket,
        static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
        (&QTcpSocket::error), this, &Connectivity::onSocketError
    );
}

void Connectivity::connectToServer(int timeout)
{
    time = QTime::currentTime();
    timer.start(timeout);
    socket.connectToHost(address, port);
}

void Connectivity::startLagTest(int timeout)
{
    testingConnectivity = false;
    connectToServer(timeout);
}

void Connectivity::startConnectivityTest(const std::string &method,
    const std::string &password, int timeout)
{
    testingConnectivity = true;
    encryptionMethod = method;
    encryptionPassword = password;
    connectToServer(timeout);
}

void Connectivity::onTimeout()
{
    socket.abort();
    emit connectivityFinished(false);
    emit lagTestFinished(LAG_TIMEOUT);
}

void Connectivity::onSocketError(QAbstractSocket::SocketError)
{
    timer.stop();
    socket.abort();
    emit connectivityFinished(false);
    emit testErrorString(socket.errorString());
    emit lagTestFinished(LAG_ERROR);
}

void Connectivity::onConnected()
{
    timer.stop();
    emit lagTestFinished(time.msecsTo(QTime::currentTime()));

    if(testingConnectivity) {
        Encryptor encryptor(encryptionMethod, encryptionPassword);
        // A http request to Google to test
        // connectivity The payload is dumped from
        // `curl http://www.google.com --socks5 127.0.0.1:1080`
        // TODO: find a better way to check connectivity
        std::string dest =
            Common::packAddress(Address("www.baidu.com", 80));
        static const QByteArray expected = QByteArray::fromHex(
                "474554202f20485454502f312e310d0a486f73743a"
                "207777772e676f6f676c652e636f6d0d0a55736572"
                "2d4167656e743a206375726c2f372e34332e300d0a"
                "4163636570743a202a2f2a0d0a0d0a"
        );
        std::string payload(expected.data(), expected.length());
        std::string toWrite = encryptor.encrypt(dest + payload);
        socket.write(toWrite.data(), toWrite.size());
    } else {
        socket.abort();
    }
}

void Connectivity::onSocketReadyRead()
{
    emit connectivityFinished(true);
    socket.abort();
}

} // namespace QSS
