#ifndef NETWORK_TCPRELAY_H
#define NETWORK_TCPRELAY_H

#include <QTime>
#include <QTimer>
#include <QObject>
#include <QTcpSocket>

#include "network/address.h"
#include "crypto/encryptor.h"

namespace QSS {

/// The abstract base class representing a Shadowsocks TCP connection
class TcpRelay : public QObject {
    Q_OBJECT
public:
    TcpRelay(QTcpSocket *localSocket, int timeout, Address server_addr,
        const std::string &method, const std::string &password
    );

    TcpRelay(const TcpRelay &) = delete;

    enum STAGE { INIT, ADDR, UDP_ASSOC, DNS, CONNECTING, STREAM, DESTROYED };

signals:
    // Count only remote socket's traffic. Either in local or server mode,
    // the remote socket is used to communicate with other-side shadowsocks
    // instance (a local or a server)
    void bytesRead(quint64);
    void bytesSend(quint64);

    // Time used for remote to connect to the host (msec)
    void latencyAvailable(int);
    void finished();

protected:
    static const int64_t RemoteRecvSize = 65536;

    STAGE stage;
    Address remoteAddress;
    Address serverAddress;
    std::string dataToWrite;

    std::unique_ptr<Encryptor> encryptor;
    std::unique_ptr<QTcpSocket> local;
    std::unique_ptr<QTcpSocket> remote;
    std::unique_ptr<QTimer> timer;
    QTime startTime;

    bool writeToRemote(const char *data, size_t length);

    virtual void handleStageAddr(std::string &data) = 0;
    virtual void handleLocalTcpData(std::string &data) = 0;
    virtual void handleRemoteTcpData(std::string &data) = 0;

protected slots:
    void onRemoteConnected();
    void onRemoteTcpSocketError();
    void onLocalTcpSocketError();
    void onLocalTcpSocketReadyRead();
    void onRemoteTcpSocketReadyRead();
    void onTimeout();
    void close();
};

} // namespace QSS

#endif // NETWORK_TCPRELAY_H
