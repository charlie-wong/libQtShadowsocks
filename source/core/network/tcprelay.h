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
    TcpRelay(QTcpSocket *local_socket, int timeout, Address server_addr,
        const std::string &method, const std::string &password
    );

    TcpRelay(const TcpRelay &) = delete;

    enum Stage { INIT, ADDR, UDP_ASSOC, DNS, CONNECTING, STREAM, DESTROYED };

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

    Stage m_stage;
    Address m_remote_addr;
    Address m_server_addr;
    std::string m_data2write;

    std::unique_ptr<Encryptor> m_encryptor;
    std::unique_ptr<QTcpSocket> m_local;
    std::unique_ptr<QTcpSocket> m_remote;
    std::unique_ptr<QTimer> m_timer;
    QTime m_start_time;

    bool writeToRemote(const char *data, size_t length);

    virtual void handleStageAddr(std::string &data) = 0;
    virtual void handleLocalTcpData(std::string &data) = 0;
    virtual void handleRemoteTcpData(std::string &data) = 0;

protected slots:
    void close();
    void onTimeout();
    void onLocalTcpSocketError();
    void onLocalTcpSocketReadyRead();
    void onRemoteConnected();
    void onRemoteTcpSocketError();
    void onRemoteTcpSocketReadyRead();
};

} // namespace QSS

#endif // NETWORK_TCPRELAY_H
