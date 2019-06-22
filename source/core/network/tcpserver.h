#ifndef NETWORK_TCPSERVER_H
#define NETWORK_TCPSERVER_H

#include <QTcpServer>
#include <list>
#include <memory>

#include "network/address.h"

namespace QSS {

class TcpRelay;

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    ~TcpServer();
    TcpServer(std::string method, std::string password, int timeout,
        bool is_local, bool auto_ban, Address server_addr
    );

    TcpServer(const TcpServer &) = delete;

signals:
    void readBytes(quint64);
    void sendBytes(quint64);
    void latencyAvailable(int);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    const bool autoBan;
    const int m_timeout;
    const bool m_is_local;
    const std::string m_method;
    const std::string m_password;
    const Address m_server_addr;

    std::list<std::shared_ptr<TcpRelay>> m_con_list;
};

} // namespace QSS

#endif // NETWORK_TCPSERVER_H
