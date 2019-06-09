#include "tcprelayserver.h"
#include "util/common.h"
#include <QDebug>
#include <utility>

namespace QSS {

TcpRelayServer::TcpRelayServer(QTcpSocket *localSocket, int timeout,
    Address server_addr, const std::string &method,
    const std::string &password, bool autoBan) :
    TcpRelay(localSocket, timeout, server_addr, method, password)
    , autoBan(autoBan)
{
    // Nothing Todo
}

void TcpRelayServer::handleStageAddr(std::string &data)
{
    int header_length = 0;
    Common::parseHeader(data, remoteAddress, header_length);

    if(header_length == 0) {
        qCritical("Can't parse header. Wrong encryption method or password?");

        if(autoBan) {
            Common::banAddress(local->peerAddress());
        }

        close();
        return;
    }

    QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
        << "Connecting " << remoteAddress << " from "
        << local->peerAddress().toString() << ":" << local->peerPort();

    stage = DNS;

    if(data.size() > header_length) {
        data = data.substr(header_length);
        dataToWrite += data;
    }

    remoteAddress.lookUp([this](bool success) {
        if(success) {
            stage = CONNECTING;
            startTime = QTime::currentTime();
            remote->connectToHost(remoteAddress.getFirstIP(),
                remoteAddress.getPort()
            );
        } else {
            QDebug(QtMsgType::QtDebugMsg).noquote()
                << "Failed to lookup remote address. Closing TCP connection.";
            close();
        }
    });
}

void TcpRelayServer::handleLocalTcpData(std::string &data)
{
    try {
        data = encryptor->decrypt(data);
    } catch(const std::exception &e) {
        QDebug(QtMsgType::QtCriticalMsg) << "Local:" << e.what();
        close();
        return;
    }

    if(data.empty()) {
        qWarning("Data is empty after decryption.");
        return;
    }

    if(stage == STREAM) {
        writeToRemote(data.data(), data.size());
    } else if(stage == CONNECTING || stage == DNS) {
        // take DNS into account, otherwise some data will get lost
        dataToWrite += data;
    } else if(stage == INIT) {
        handleStageAddr(data);
    } else {
        qCritical("Local unknown stage.");
    }
}

void TcpRelayServer::handleRemoteTcpData(std::string &data)
{
    data = encryptor->encrypt(data);
}

}  // namespace QSS
