#ifndef SQPROFILE_H
#define SQPROFILE_H

#include <QDataStream>
#include <QDate>
#include <QDateTime>
#include <QtShadowsocks>

struct SQProfile {
    SQProfile();
    SQProfile(const QSS::Profile &profile); // Copy values from QSS Profile
    SQProfile(const QString &uri); // Construct it using ss protocol

    QSS::Profile toProfile() const; // Convert it into a QSS Profile

    bool autoStart;
    bool debug;
    quint16 serverPort;
    quint16 localPort;
    QString name;
    QString serverAddress;
    QString localAddress;
    QString method;
    QString password;
    int timeout;
    int latency;
    quint64 currentUsage;
    quint64 totalUsage;
    QDateTime lastTime; // last time this connection is used
    QDate nextResetDate; // next scheduled date to reset data usage
    bool httpMode;

    static const int LATENCY_TIMEOUT = -1;
    static const int LATENCY_ERROR = -2;
    static const int LATENCY_UNKNOWN = -3;
};

Q_DECLARE_METATYPE(SQProfile)

QDataStream &operator << (QDataStream &out, const SQProfile &p);
QDataStream &operator >> (QDataStream &in, SQProfile &p);

#endif // SQPROFILE_H
