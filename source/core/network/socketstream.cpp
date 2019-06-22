#include "socketstream.h"

using namespace QSS;

SocketStream::SocketStream(QAbstractSocket *a, QAbstractSocket *b,
    QObject *parent) : QObject(parent), m_as(a), m_bs(b)
{
    connect(m_as, &QAbstractSocket::readyRead,
        this, &SocketStream::onSocketAReadyRead
    );
    connect(m_bs, &QAbstractSocket::readyRead,
        this, &SocketStream::onSocketBReadyRead
    );
}

void SocketStream::onSocketAReadyRead()
{
    if(m_bs->isWritable()) {
        m_bs->write(m_as->readAll());
    } else {
        qCritical("The second socket is not writable");
    }
}

void SocketStream::onSocketBReadyRead()
{
    if(m_as->isWritable()) {
        m_as->write(m_bs->readAll());
    } else {
        qCritical("The first socket is not writable");
    }
}
