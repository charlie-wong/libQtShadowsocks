#include <QDebug>
#include <utility>

#include "tcprelay.h"
#include "utils/common.h"

namespace QSS {

TcpRelay::TcpRelay(QTcpSocket *local_socket, int timeout, Address server_addr,
    const std::string &method, const std::string &password) :
    m_stage(INIT)
    , m_server_addr(std::move(server_addr))
    , m_encryptor(new Encryptor(method, password))
    , m_local(local_socket)
    , m_remote(new QTcpSocket())
    , m_timer(new QTimer())
{
    m_timer->setInterval(timeout);
    connect(m_timer.get(), &QTimer::timeout, this, &TcpRelay::onTimeout);
    connect(m_local.get(),
        static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
        (&QTcpSocket::error),
        this, &TcpRelay::onLocalTcpSocketError
    );
    connect(m_local.get(), &QTcpSocket::disconnected, this, &TcpRelay::close);
    connect(m_local.get(), &QTcpSocket::readyRead,
        this, &TcpRelay::onLocalTcpSocketReadyRead
    );
    connect(m_local.get(), &QTcpSocket::readyRead,
        m_timer.get(), static_cast<void (QTimer::*)()> (&QTimer::start)
    );
    connect(m_remote.get(), &QTcpSocket::connected,
        this, &TcpRelay::onRemoteConnected
    );
    connect(m_remote.get(),
        static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
        (&QTcpSocket::error),
        this, &TcpRelay::onRemoteTcpSocketError
    );
    connect(m_remote.get(), &QTcpSocket::disconnected, this, &TcpRelay::close);
    connect(m_remote.get(), &QTcpSocket::readyRead,
        this, &TcpRelay::onRemoteTcpSocketReadyRead
    );
    connect(m_remote.get(), &QTcpSocket::readyRead,
        m_timer.get(), static_cast<void (QTimer::*)()> (&QTimer::start)
    );
    connect(m_remote.get(), &QTcpSocket::bytesWritten,
        this, &TcpRelay::sendBytes
    );
    m_local->setReadBufferSize(RemoteRecvSizeMax);
    m_local->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_local->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_remote->setReadBufferSize(RemoteRecvSizeMax);
    m_remote->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_remote->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

void TcpRelay::close()
{
    if(m_stage == DESTROYED) {
        return;
    }

    m_local->close();
    m_remote->close();
    m_stage = DESTROYED;
    emit finished();
}

void TcpRelay::onLocalTcpSocketError()
{
    // it's not an "error" if remote host closed a connection
    if(m_local->error() != QAbstractSocket::RemoteHostClosedError) {
        QDebug(QtMsgType::QtWarningMsg).noquote()
            << "Local socket:" << m_local->errorString();
    } else {
        QDebug(QtMsgType::QtDebugMsg).noquote()
            << "Local socket:" << m_local->errorString();
    }

    close();
}

bool TcpRelay::writeToRemote(const char *data, size_t length)
{
    return m_remote->write(data, length) != -1;
}

void TcpRelay::onRemoteConnected()
{
    m_stage = STREAM;
    emit latencyAvailable(m_start_time.msecsTo(QTime::currentTime()));

    if(!m_data2write.empty()) {
        writeToRemote(m_data2write.data(), m_data2write.size());
        m_data2write.clear();
    }
}

void TcpRelay::onRemoteTcpSocketError()
{
    // it's not an "error" if remote host closed a connection
    if(m_remote->error() != QAbstractSocket::RemoteHostClosedError) {
        QDebug(QtMsgType::QtWarningMsg).noquote()
            << "Remote socket:" << m_remote->errorString();
    } else {
        QDebug(QtMsgType::QtDebugMsg).noquote()
            << "Remote socket:" << m_remote->errorString();
    }

    close();
}

void TcpRelay::onLocalTcpSocketReadyRead()
{
    std::string data;
    data.resize(RemoteRecvSizeMax);
    int64_t readSize = m_local->read(&data[0], data.size());

    if(readSize == -1) {
        qCritical("Attempted to read from closed local socket.");
        close();
        return;
    }

    data.resize(readSize);

    if(data.empty()) {
        qCritical("Local received empty data.");
        close();
        return;
    }

    handleLocalTcpData(data);
}

void TcpRelay::onRemoteTcpSocketReadyRead()
{
    std::string buf;
    buf.resize(RemoteRecvSizeMax);
    int64_t readSize = m_remote->read(&buf[0], buf.size());

    if(readSize == -1) {
        qCritical("Attempted to read from closed remote socket.");
        close();
        return;
    }

    buf.resize(readSize);

    if(buf.empty()) {
        qWarning("Remote received empty data.");
        close();
        return;
    }

    emit readBytes(buf.size());

    try {
        handleRemoteTcpData(buf);
    } catch(const std::exception &e) {
        QDebug(QtMsgType::QtCriticalMsg).noquote() << "Remote:" << e.what();
        close();
        return;
    }

    m_local->write(buf.data(), buf.size());
}

void TcpRelay::onTimeout()
{
    qInfo("TCP connection timeout.");
    close();
}

}  // namespace QSS
