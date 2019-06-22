#ifndef NETWORK_TCPRELAYSERVER_H
#define NETWORK_TCPRELAYSERVER_H

#include "tcprelay.h"

namespace QSS {

class TcpRelayServer : public TcpRelay {
    Q_OBJECT
public:
    TcpRelayServer(QTcpSocket *local_socket, int timeout,
        Address server_addr, const std::string &method,
        const std::string &password, bool autoBan
    );

protected:
    const bool autoBan;

    void handleStageAddr(std::string &data) final;
    void handleLocalTcpData(std::string &data) final;
    void handleRemoteTcpData(std::string &data) final;
};

} // namespace QSS

#endif // NETWORK_TCPRELAYSERVER_H
