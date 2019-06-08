#ifndef UDPRELAY_H
#define UDPRELAY_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <map>
#include "types/address.h"
#include "crypto/encryptor.h"

namespace QSS {

class QSS_EXPORT UdpRelay : public QObject
{
    Q_OBJECT
public:
    UdpRelay(const std::string& method,
             const std::string& password,
             bool is_local,
             bool auto_ban,
             Address serverAddress);

    UdpRelay(const UdpRelay &) = delete;

    bool isListening() const;

public slots:
    bool listen(const QHostAddress& addr, uint16_t port);
    void close();

signals:
    /*
     * The same situation here.
     * We only count "listen" socket's read and written bytes
     */
    void bytesRead(quint64);
    void bytesSend(quint64);

private:
    //64KB, same as shadowsocks-python (udprelay)
    static const int64_t RemoteRecvSize = 65536;

    const Address serverAddress;
    const bool isLocal;
    const bool autoBan;
    QUdpSocket listenSocket;
    std::unique_ptr<Encryptor> encryptor;

    std::map<Address, std::shared_ptr<QUdpSocket> > m_cache;

private slots:
    void onSocketError();
    void onListenStateChanged(QAbstractSocket::SocketState);
    void onServerUdpSocketReadyRead();
};

}

#endif // UDPRELAY_H
