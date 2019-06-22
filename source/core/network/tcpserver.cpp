#include "tcprelayclient.h"
#include <QDebug>
#include <utility>

#include "utils/common.h"
#include "tcpserver.h"
#include "tcprelayserver.h"

namespace QSS {

TcpServer::TcpServer(std::string method, std::string password, int timeout,
    bool is_local, bool auto_ban, Address server_addr) :
    autoBan(auto_ban)
    , m_timeout(timeout)
    , m_is_local(is_local)
    , m_method(std::move(method))
    , m_password(std::move(password))
    , m_server_addr(std::move(server_addr))

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

    if(!m_is_local && autoBan
       && Common::isAddressBanned(local_socket->peerAddress())) {
        QDebug(QtMsgType::QtInfoMsg).noquote()
            << "A banned IP" << local_socket->peerAddress()
            << "attempted to access this server";
        return;
    }

    // timeout * 1000: convert sec to msec
    std::shared_ptr<TcpRelay> con;

    if(m_is_local) {
        con = std::make_shared<TcpRelayClient> (local_socket.release(),
            m_timeout * 1000, m_server_addr, m_method, m_password
        );
    } else {
        con = std::make_shared<TcpRelayServer>(local_socket.release(),
            m_timeout * 1000, m_server_addr, m_method, m_password, autoBan
        );
    }

    m_con_list.push_back(con);
    connect(con.get(), &TcpRelay::readBytes, this, &TcpServer::readBytes);
    connect(con.get(), &TcpRelay::sendBytes, this, &TcpServer::sendBytes);
    connect(con.get(), &TcpRelay::latencyAvailable,
        this, &TcpServer::latencyAvailable
    );
    connect(con.get(), &TcpRelay::finished, this, [con, this]() {
        m_con_list.remove(con);
    });
}

}  // namespace QSS
