#include "socketstream.h"

using namespace QSS;

SocketStream::SocketStream(QAbstractSocket *a, QAbstractSocket *b,
    QObject *parent) : QObject(parent), as(a), bs(b)
{
    connect(as, &QAbstractSocket::readyRead,
        this, &SocketStream::onSocketAReadyRead
    );
    connect(bs, &QAbstractSocket::readyRead,
        this, &SocketStream::onSocketBReadyRead
    );
}

void SocketStream::onSocketAReadyRead()
{
    if(bs->isWritable()) {
        bs->write(as->readAll());
    } else {
        qCritical("The second socket is not writable");
    }
}

void SocketStream::onSocketBReadyRead()
{
    if(as->isWritable()) {
        as->write(bs->readAll());
    } else {
        qCritical("The first socket is not writable");
    }
}
