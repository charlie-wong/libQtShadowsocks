#ifndef UTILS_CONTROLLER_H
#define UTILS_CONTROLLER_H

#include <QObject>
#include <QHostAddress>

#include "export.h"
#include "network/tcpserver.h"
#include "network/httpproxy.h"
#include "types/profile.h"
#include "network/udprelay.h"

namespace QSS {

class QSS_EXPORT Controller : public QObject {
    Q_OBJECT
public:
    Controller(Profile _profile, bool is_local,
        bool auto_ban, QObject *parent = 0
    );
    ~Controller();

    Controller(const Controller &) = delete;

signals:
    // Connect this signal to get notified when running state is changed
    void runningStateChanged(bool);

    // These two signals pass any new bytes read or sent
    void newBytesReceived(quint64);
    void newBytesSent(quint64);

    // These two signals pass accumulated bytes read or sent so far,
    // aka total in this session
    void bytesReceivedChanged(quint64);
    void bytesSentChanged(quint64);

    // Time used to connect to remote host (msec)
    // remote host means shadowsocks server if this controller is in local
    // mode, or the address the client is accessing if it's in server mode.
    void tcpLatencyAvailable(int);

public slots:
    bool start(); // Return true if start successfully, otherwise return false
    void stop();

protected:
    // The total bytes recevied or sent by/from all TCP and UDP connections.
    uint64_t bytesReceived;
    uint64_t bytesSent;

    Profile profile;
    Address serverAddress;
    // Run on local-side (client) or server-side (server)
    const bool isLocal;

    // auto ban IPs that use malformed header data as our
    // anti-probe measure (only used when it's a server)
    const bool autoBan;
    std::unique_ptr<TcpServer> tcpServer;
    std::unique_ptr<UdpRelay> udpRelay;
    std::unique_ptr<HttpProxy> httpProxy;

    QHostAddress getLocalAddr();

protected slots:
    void onTcpServerError(QAbstractSocket::SocketError err);
    void onBytesRead(quint64);
    void onBytesSend(quint64);
};

}
#endif // UTILS_CONTROLLER_H
