#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QObject>
#include <QStringList>

#include "connection.h"

class ConnectionItem : public QObject {
    Q_OBJECT

public:
    explicit ConnectionItem(Connection *_con, QObject *parent = nullptr);

    static int columnCount();
    QVariant data(int column, int role = Qt::DisplayRole) const;

    Connection *getConnection();
    void testLatency();

signals:
    void message(const QString &);
    void stateChanged(bool);
    void dataUsageChanged(const quint64 &current, const quint64 &total);
    void latencyChanged();

private:
    Connection *con;

    static QString convertLatencyToString(const int latency);
    static QString convertBytesToHumanReadable(quint64 bytes);
    static const QStringList bytesUnits;

private slots:
    void onConnectionStateChanged(bool running);
    void onConnectionPingFinished(const int latency);
    void onStartFailed();
};

#endif // CONNECTIONITEM_H
