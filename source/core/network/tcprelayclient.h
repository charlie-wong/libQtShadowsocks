#ifndef NETWORK_TCPRELAYCLIENT_H
#define NETWORK_TCPRELAYCLIENT_H

#include "tcprelay.h"

namespace QSS {

class TcpRelayClient : public TcpRelay {
    Q_OBJECT
public:
    TcpRelayClient(QTcpSocket *local_socket, int timeout,
        Address server_addr, const std::string &method,
        const std::string &password
    );

protected:
    void handleStageAddr(std::string &data) final;
    void handleLocalTcpData(std::string &data) final;
    void handleRemoteTcpData(std::string &data) final;
};

}
#endif // NETWORK_TCPRELAYCLIENT_H
