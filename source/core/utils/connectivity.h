// Perform non-blocking connectivity tests

#ifndef UTILS_CONNECTIVITY_H
#define UTILS_CONNECTIVITY_H

#include <QTime>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>

namespace QSS {

// This class is only meaningful for client-side applications
class Connectivity : public QObject {
    Q_OBJECT
public:
    Connectivity(const QHostAddress &server_addr,
        const uint16_t &server_port, QObject *parent = 0
    );

    Connectivity(const Connectivity &) = delete;

    static const int LAG_ERROR = -2;
    static const int LAG_TIMEOUT = -1;

    // Connectivity test will try to establish a shadowsocks connection
    // with the server. The result is passed by signal connTestFinished().
    // If the server times out, the connectivity will be passed as false.
    //
    // Calling this function does lag (latency) test as well. Therefore, it's
    // the recommended way to do connectivity and latency test with just one
    // function call.
    //
    // Don't call the same Connectivity instance's connTestStart()
    // and lagTestStart() at the same time!
    void connTestStart(const std::string &method,
        const std::string &password, int timeout = 3000
    );

signals:
    void lagTestFinished(int);
    void connTestFinished(bool);
    void testResult(const QString &);

public slots:
    // The lag test only tests if the server port is open and listeninig
    // bind lagTestFinished() signal to get the test result
    void lagTestStart(int timeout = 3000); // 3000 msec by default

private:
    void connectToServer(int timeout);

private:
    uint16_t m_server_port;
    QHostAddress m_server_addr;

    QTimer m_timer;
    QTime m_timestamp;

    QTcpSocket m_socket;
    bool m_do_connectivity_test;

    std::string encryptionMethod;
    std::string encryptionPassword;

private slots:
    void onTimeout();
    void onSocketError(QAbstractSocket::SocketError);
    void onConnected();
    void onSocketReadyRead();
};

}
#endif // UTILS_CONNECTIVITY_H
