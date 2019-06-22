#include <QDebug>
#include <utility>

#include "utils/common.h"
#include "tcprelayserver.h"

namespace QSS {

TcpRelayServer::TcpRelayServer(QTcpSocket *local_socket, int timeout,
    Address server_addr, const std::string &method,
    const std::string &password, bool autoBan) :
    TcpRelay(local_socket, timeout, server_addr, method, password)
    , autoBan(autoBan)
{
    // Nothing Todo
}

void TcpRelayServer::handleStageAddr(std::string &data)
{
    int header_length = 0;
    Common::parseHeader(data, m_remote_addr, header_length);

    if(header_length == 0) {
        qCritical("Can't parse header. Wrong encryption method or password?");

        if(autoBan) {
            Common::banAddress(m_local->peerAddress());
        }

        close();
        return;
    }

    m_stage = DNS;
    QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
        << "Connecting " << m_remote_addr << " from "
        << m_local->peerAddress().toString() << ":" << m_local->peerPort();

    if(data.size() > static_cast<size_t>(header_length)) {
        data = data.substr(header_length);
        m_data2write += data;
    }

    m_remote_addr.lookUp([this](bool success) {
        if(success) {
            m_stage = CONNECTING;
            m_start_time = QTime::currentTime();
            m_remote->connectToHost(m_remote_addr.getFirstIP(),
                m_remote_addr.getPort()
            );
        } else {
            qDebug() << "Failed to lookup remote address. Closing TCP connection.";
            close();
        }
    });
}

void TcpRelayServer::handleLocalTcpData(std::string &data)
{
    try {
        data = m_encryptor->decrypt(data);
    } catch(const std::exception &e) {
        QDebug(QtMsgType::QtCriticalMsg).noquote() << "Local:" << e.what();
        close();
        return;
    }

    if(data.empty()) {
        qWarning("Data is empty after decryption.");
        return;
    }

    if(m_stage == STREAM) {
        writeToRemote(data.data(), data.size());
    } else if(m_stage == CONNECTING || m_stage == DNS) {
        // take DNS into account, otherwise some data will get lost
        m_data2write += data;
    } else if(m_stage == INIT) {
        handleStageAddr(data);
    } else {
        qCritical("Local unknown stage.");
    }
}

void TcpRelayServer::handleRemoteTcpData(std::string &data)
{
    data = m_encryptor->encrypt(data);
}

}  // namespace QSS
