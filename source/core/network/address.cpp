// Communicate with lower-level encrytion library

#include "address.h"
#include "utils/common.h"

namespace  QSS {

void DnsLookup::lookup(const QString &hostname)
{
    QHostInfo::lookupHost(hostname, this, SLOT(lookedUp(QHostInfo)));
}

const QList<QHostAddress> DnsLookup::iplist() const
{
    return m_ips;
}

void DnsLookup::lookedUp(const QHostInfo &info)
{
    if(info.error() != QHostInfo::NoError) {
        qWarning("DNS lookup failed: %s",
            info.errorString().toStdString().data()
        );
    } else {
        m_ips = info.addresses();
    }

    emit finished();
}

Address::Address(const std::string &a, uint16_t p)
{
    m_ap.second = p;
    setAddress(a);
}

Address::Address(const QHostAddress &ip, uint16_t p)
{
    m_ap.second = p;
    setIPAddress(ip);
}

const std::string &Address::getAddress() const
{
    return m_ap.first;
}

QHostAddress Address::getRandomIP() const
{
    if(m_ipaddrs.empty()) {
        return QHostAddress();
    }

    return m_ipaddrs.at(Common::randomNumber(m_ipaddrs.size()));
}

QHostAddress Address::getFirstIP() const
{
    return m_ipaddrs.empty() ? QHostAddress() : m_ipaddrs.front();
}

bool Address::isIPValid() const
{
    return !m_ipaddrs.empty();
}

uint16_t Address::getPort() const
{
    return m_ap.second;
}

void Address::lookUp(Address::LookUpCallback cb)
{
    if(isIPValid()) {
        return cb(true);
    }

    if(m_dns) {
        return; // DNS lookup is in-progress
    }

    m_dns = std::make_shared<DnsLookup>();
    QObject::connect(m_dns.get(), &DnsLookup::finished, [cb, this]() {
        m_ipaddrs = m_dns->iplist().toVector().toStdVector();
        cb(!m_ipaddrs.empty());
        m_dns.reset();
    });
    m_dns->lookup(QString::fromStdString(m_ap.first));
}

bool Address::blockingLookUp()
{
    if(!isIPValid()) {
        QHostInfo result =
            QHostInfo::fromName(QString::fromStdString(m_ap.first));

        if(result.error() != QHostInfo::NoError) {
            qDebug("Failed to look up host address: %s",
                result.errorString().toStdString().data()
            );
            return false;
        }

        m_ipaddrs = result.addresses().toVector().toStdVector();
    }

    return true;
}

void Address::setAddress(const std::string &a)
{
    m_ap.first = QString::fromStdString(a).trimmed().toStdString();
    m_ipaddrs.clear();
    QHostAddress ipAddress(QString::fromStdString(a));

    if(!ipAddress.isNull()) {
        m_ipaddrs.push_back(ipAddress);
    }
}

void Address::setIPAddress(const QHostAddress &ip)
{
    m_ipaddrs.clear();
    m_ipaddrs.push_back(ip);
    m_ap.first = ip.toString().toStdString();
}

void Address::setPort(uint16_t p)
{
    m_ap.second = p;
}

Address::AddrType Address::addrType() const
{
    QHostAddress ipAddress(QString::fromStdString(m_ap.first));

    if(ipAddress.isNull()) {
        return HOST; // it's a domain if it can't be parsed
    }

    if(ipAddress.protocol() == QAbstractSocket::IPv4Protocol) {
        return IPV4;
    }

    return IPV6;
}

std::string Address::toString() const
{
    return m_ap.first + ":" + std::to_string(m_ap.second);
}

}  // namespace QSS
