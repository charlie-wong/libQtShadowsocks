#include <QDebug>
#include <utility>

#include "utils/common.h"
#include "tcprelayclient.h"

namespace QSS {

TcpRelayClient::TcpRelayClient(QTcpSocket *localSocket, int timeout,
    Address server_addr, const std::string &method,
    const std::string &password):
    TcpRelay(localSocket, timeout, server_addr, method, password)
{
    // Nothing Todo
}

void TcpRelayClient::handleStageAddr(std::string &data)
{
    auto cmd = static_cast<int>(data.at(1));

    if(cmd == 3) { // CMD_UDP_ASSOCIATE
        qDebug("UDP associate");
        static const char header_data [] = { 5, 0, 0 };
        QHostAddress addr = local->localAddress();
        uint16_t port = local->localPort();
        std::string toWrite =
            std::string(header_data, 3) + Common::packAddress(addr, port);
        local->write(toWrite.data(), toWrite.length());
        stage = UDP_ASSOC;
        return;
    }

    if(cmd == 1) { // CMD_CONNECT
        data = data.substr(3);
    } else {
        qCritical("Unknown command %d", cmd);
        close();
        return;
    }

    int header_length = 0;
    Common::parseHeader(data, remoteAddress, header_length);

    if(header_length == 0) {
        qCritical("Can't parse header. Wrong encryption method or password?");
        close();
        return;
    }

    QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
        << "Connecting " << remoteAddress << " from "
        << local->peerAddress().toString() << ":" << local->peerPort();

    stage = DNS;
    static constexpr const char res [] = { 5, 0, 0, 1, 0, 0, 0, 0, 16, 16 };
    static const QByteArray response(res, 10);
    local->write(response);
    dataToWrite += encryptor->encrypt(data);
    serverAddress.lookUp([this](bool success) {
        if(success) {
            stage = CONNECTING;
            startTime = QTime::currentTime();
            remote->connectToHost(
                serverAddress.getFirstIP(), serverAddress.getPort()
            );
        } else {
            QDebug(QtMsgType::QtDebugMsg).noquote()
                << "Failed to lookup server address. Closing TCP connection.";
            close();
        }
    });
}

void TcpRelayClient::handleLocalTcpData(std::string &data)
{
    switch(stage) {
        case STREAM:
            data = encryptor->encrypt(data);
            writeToRemote(data.data(), data.size());
            break;

        case INIT: {
            static constexpr const char reject_data [] = { 0, 91 };
            static constexpr const char accept_data [] = { 5, 0 };
            static const QByteArray reject(reject_data, 2);
            static const QByteArray accept(accept_data, 2);

            if(data[0] != char(5)) {
                qCritical("An invalid socket connection was rejected. "
                    "Please make sure the connection type is SOCKS5."
                );
                local->write(reject);
            } else {
                local->write(accept);
            }

            stage = ADDR;
            break;
        }

        case CONNECTING:
        case DNS:
            // take DNS into account, otherwise some data will get lost
            dataToWrite += encryptor->encrypt(data);
            break;

        case ADDR:
            handleStageAddr(data);
            break;

        default:
            qCritical("Local unknown stage.");
    }
}

void TcpRelayClient::handleRemoteTcpData(std::string &data)
{
    data = encryptor->decrypt(data);
}

}  // namespace QSS
