#include "tcprelayclient.h"
#include <QDebug>
#include <utility>

#include "utils/common.h"
#include "tcpserver.h"
#include "tcprelayserver.h"

namespace QSS {

TcpServer::TcpServer(std::string method, std::string password, int timeout,
    bool is_local, bool auto_ban, Address serverAddress) :
    method(std::move(method))
    , password(std::move(password))
    , isLocal(is_local)
    , autoBan(auto_ban)
    , serverAddress(std::move(serverAddress))
    , timeout(timeout)
{
    // Nothing Todo
}

TcpServer::~TcpServer()
{
    if(isListening()) {
        close();
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    auto local_socket = std::make_unique<QTcpSocket>();
    local_socket->setSocketDescriptor(socketDescriptor);

    if(!isLocal
       && autoBan
       && Common::isAddressBanned(local_socket->peerAddress())) {
        QDebug(QtMsgType::QtInfoMsg).noquote()
            << "A banned IP" << local_socket->peerAddress()
            << "attempted to access this server";
        return;
    }

    // timeout * 1000: convert sec to msec
    std::shared_ptr<TcpRelay> con;

    if(isLocal) {
        con = std::make_shared<TcpRelayClient> (local_socket.release(),
            timeout * 1000, serverAddress, method, password
        );
    } else {
        con = std::make_shared<TcpRelayServer>(local_socket.release(),
            timeout * 1000, serverAddress, method, password, autoBan
        );
    }

    conList.push_back(con);
    connect(con.get(), &TcpRelay::bytesRead, this, &TcpServer::bytesRead);
    connect(con.get(), &TcpRelay::bytesSend, this, &TcpServer::bytesSend);
    connect(con.get(), &TcpRelay::latencyAvailable,
        this, &TcpServer::latencyAvailable
    );
    connect(con.get(), &TcpRelay::finished, this, [con, this]() {
        conList.remove(con);
    });
}

}  // namespace QSS
