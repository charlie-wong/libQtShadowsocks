#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include <QObject>
#include <QAbstractSocket>
#include "util/export.h"

namespace QSS {

class QSS_EXPORT SocketStream : public QObject {
    Q_OBJECT
public:
    // A light-weight class dedicated to stream data between two sockets
    // all available data from socket a will be written to socket b vice versa
    SocketStream(QAbstractSocket *a, QAbstractSocket *b, QObject *parent = 0);

    SocketStream(const SocketStream &) = delete;

private:
    QAbstractSocket *as;
    QAbstractSocket *bs;

private slots:
    void onSocketAReadyRead();
    void onSocketBReadyRead();
};

} // namespace QSS

#endif // SOCKETSTREAM_H