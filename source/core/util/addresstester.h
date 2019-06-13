// Perform non-blocking address tests

#ifndef ADDRESSTESTER_H
#define ADDRESSTESTER_H

#include "export.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

namespace QSS {

// This class is only meaningful for client-side applications
class QSS_EXPORT AddressTester : public QObject {
    Q_OBJECT
public:
    AddressTester(const QHostAddress &server_address,
        const uint16_t &server_port, QObject *parent = 0
    );

    AddressTester(const AddressTester &) = delete;

    static const int LAG_TIMEOUT = -1;
    static const int LAG_ERROR = -2;

    // Connectivity test will try to establish a shadowsocks connection with
    // the server. The result is passed by signal connectivityTestFinished().
    // If the server times out, the connectivity will be passed as false.
    //
    // Calling this function does lag (latency) test as well. Therefore, it's
    // the recommended way to do connectivity and latency test with just one
    // function call.
    //
    // Don't call the same AddressTester instance's startConnectivityTest()
    // and startLagTest() at the same time!
    void startConnectivityTest(const std::string &method,
        const std::string &password, int timeout = 3000
    );

signals:
    void lagTestFinished(int);
    void testErrorString(const QString &);
    void connectivityTestFinished(bool);

public slots:
    // The lag test only tests if the server port is open and listeninig
    // bind lagTestFinished() signal to get the test result
    void startLagTest(int timeout = 3000); // 3000 msec by default

private:
    QHostAddress address;
    uint16_t port;
    QTime time;
    QTcpSocket socket;
    QTimer timer;
    bool testingConnectivity;

    std::string encryptionMethod;
    std::string encryptionPassword;

    void connectToServer(int timeout);

private slots:
    void onTimeout();
    void onSocketError(QAbstractSocket::SocketError);
    void onConnected();
    void onSocketReadyRead();
};

}
#endif // ADDRESSTESTER_H
