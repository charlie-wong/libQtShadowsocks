#ifndef NETWORK_UDPRELAY_H
#define NETWORK_UDPRELAY_H

#include <map>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

#include "network/address.h"
#include "crypto/encryptor.h"

namespace QSS {

class UdpRelay : public QObject {
    Q_OBJECT
public:
    UdpRelay(const UdpRelay &) = delete;
    UdpRelay(const std::string &method, const std::string &password,
        bool is_local, bool auto_ban, Address serverAddress
    );

    bool isListening() const;

public slots:
    void close();
    bool listen(const QHostAddress &addr, uint16_t port);

signals:
    // The same situation here.
    // We only count "listen" socket's read and written bytes
    void bytesRead(quint64);
    void bytesSend(quint64);

private:
    // 64KB, same as shadowsocks-python (udprelay)
    static const int64_t RemoteRecvSizeMax = 65536;

    const bool isLocal;
    const bool autoBan;
    QUdpSocket listenSocket;
    const Address serverAddress;
    std::unique_ptr<Encryptor> encryptor;

    std::map<Address, std::shared_ptr<QUdpSocket> > m_cache;

private slots:
    void onSocketError();
    void onListenStateChanged(QAbstractSocket::SocketState);
    void onServerUdpSocketReadyRead();
};

} // namespace QSS

#endif // NETWORK_UDPRELAY_H
