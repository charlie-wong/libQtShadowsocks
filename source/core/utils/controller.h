#ifndef UTILS_CONTROLLER_H
#define UTILS_CONTROLLER_H

#include <QObject>
#include <QHostAddress>

#include "utils/profile.h"
#include "network/udprelay.h"
#include "network/tcpserver.h"
#include "network/httpproxy.h"

namespace QSS {

class Controller : public QObject {
    Q_OBJECT
public:
    ~Controller();
    Controller(const Controller &) = delete;

    Controller(Profile profile, bool is_local,
        bool auto_ban, QObject *parent=0
    );

signals:
    // Connect this signal to get notified when running state is changed
    void runningStateChanged(bool);

    // These two signals pass any new bytes read or sent
    void newBytesSent(quint64);
    void newBytesReceived(quint64);

    // These two signals pass accumulated bytes
    // read or sent so far, aka total in this session
    void bytesSentChanged(quint64);
    void bytesReceivedChanged(quint64);

    // Time used to connect to remote host (msec)
    // remote host means shadowsocks server if this controller is in local
    // mode, or the address the client is accessing if it's in server mode.
    void onTcpLatencyAvailable(int);

public slots:
    void stop();
    bool start(); // true if successfully, otherwise false

private:
    QHostAddress getLocalAddr();

    // auto ban IPs that use malformed header data as our
    // anti-probe measure (only used when it's a server)
    const bool autoBan;

    // The total bytes recevied or sent by/from all TCP and UDP connections.
    uint64_t m_sent_bytes;
    uint64_t m_received_bytes;

    Profile m_profile;
    Address m_server_addr;
    // Run on local-side (client) or server-side (server)
    const bool m_is_local;

    std::unique_ptr<UdpRelay> m_udp_relay;
    std::unique_ptr<TcpServer> m_tcp_server;
    std::unique_ptr<HttpProxy> m_http_proxy;

private slots:
    void onReadBytes(quint64);
    void onSendBytes(quint64);
    void onTcpServerError(QAbstractSocket::SocketError err);
};

}
#endif // UTILS_CONTROLLER_H
