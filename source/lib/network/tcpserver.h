#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <list>
#include <memory>
#include "types/address.h"
#include "util/export.h"

namespace QSS {

class TcpRelay;

class QSS_EXPORT TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(std::string method,
              std::string password,
              int timeout,
              bool is_local,
              bool auto_ban,
              Address serverAddress);
    ~TcpServer();

    TcpServer(const TcpServer &) = delete;

signals:
    void bytesRead(quint64);
    void bytesSend(quint64);
    void latencyAvailable(int);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    const std::string method;
    const std::string password;
    const bool isLocal;
    const bool autoBan;
    const Address serverAddress;
    const int timeout;

    std::list<std::shared_ptr<TcpRelay> > conList;
};

}

#endif // TCPSERVER_H
