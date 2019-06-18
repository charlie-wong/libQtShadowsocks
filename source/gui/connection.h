#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include <QObject>
#include <QtShadowsocks>

#include "sqprofile.h"

class Connection : public QObject {
    Q_OBJECT
public: Connection(QObject *parent = 0);
    Connection(const SQProfile &_profile, QObject *parent = 0);
    Connection(QString uri, QObject *parent = 0);
    ~Connection();

    Connection(const Connection &) = delete;
    Connection(Connection &&) = default;

    const SQProfile &getProfile() const;
    const QString &getName() const;
    QByteArray getURI() const;
    bool isValid() const;
    const bool &isRunning() const;
    void latencyTest();

signals:
    void stateChanged(bool started);
    void latencyAvailable(const int);
    void newLogAvailable(const QString &);
    void dataUsageChanged(const quint64 &current, const quint64 &total);
    void startFailed();

public slots:
    void start();
    void stop();

private:
    std::unique_ptr<QSS::Controller> controller;
    SQProfile profile;
    bool running;

    void testAddressLatency(const QHostAddress &addr);

    friend class EditDialog;
    friend class ConfigHelper;
    friend class StatusDialog;
    friend class ConnectionItem;

private slots:
    void onNewBytesTransmitted(const quint64 &);
    void onServerAddressLookedUp(const QHostInfo &host);
    void onLatencyAvailable(const int);
    void onConnectivityTestFinished(bool);
};

Q_DECLARE_METATYPE(Connection *)

#endif // CONNECTION_H
