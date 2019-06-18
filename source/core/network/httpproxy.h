// This class enables transparent HTTP proxy that handles data transfer
// and send/recv them via upper-level SOCKS5 proxy

#ifndef NETWORK_HTTPPROXY_H
#define NETWORK_HTTPPROXY_H

#include <QTcpServer>
#include <QNetworkProxy>

namespace QSS {

class HttpProxy : public QTcpServer {
    Q_OBJECT
public:
    HttpProxy();

    HttpProxy(const HttpProxy &) = delete;

    // DO NOT use listen() function, use httpListen instead
    // The socks_port is local socks proxy server port
    bool httpListen(const QHostAddress &http_addr,
        uint16_t http_port, uint16_t socks_port
    );

protected:
    void incomingConnection(qintptr handle);

private:
    QNetworkProxy upstreamProxy;

private slots:
    void onSocketError(QAbstractSocket::SocketError);
    void onSocketReadyRead();
    void onProxySocketConnected();
    // This function is used for HTTPS transparent proxy
    void onProxySocketConnectedHttps();
    void onProxySocketReadyRead();
};

} // namespace QSS

#endif // NETWORK_HTTPPROXY_H
