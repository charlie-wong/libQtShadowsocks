// Communicate with lower-level encrytion library

#ifndef ADDRESS_H
#define ADDRESS_H

#include <QString>
#include <QHostAddress>
#include <QHostInfo>

#include <functional>
#include <memory>
#include <vector>

#include "utils/export.h"

namespace QSS {

class QSS_EXPORT DnsLookup : public QObject {
    // A simple wrapper class to provide asynchronous DNS lookup
    Q_OBJECT
public:
    void lookup(const QString &hostname);
    const QList<QHostAddress> iplist() const;

signals:
    void finished();

private slots:
    void lookedUp(const QHostInfo &info);

private:
    QList<QHostAddress> m_ips;
};

class QSS_EXPORT Address {
public:
    explicit Address(const std::string &a = std::string(), uint16_t p = 0);

    Address(const QHostAddress &ip, uint16_t p);

    Address(const Address &) = default;
    Address(Address &&) = default;

    Address &operator=(const Address &) = default;

    const std::string &getAddress() const;

    /// Because the ipAddrList might include both IPv4 and IPv6 addresses
    /// getRandomIP() will literally return a random IP address (IPv4 or IPv6)
    /// If there is no valid IP, a default constructed QHostAddress is returned.
    ///
    /// @todo: detect IPv4/IPv6 reachability automatically
    QHostAddress getRandomIP() const;

    /// Normally the order is platform-dependent and it'd consider IPv4
    /// and IPv6 precedence, which *might* be more suitable to use this
    /// function to get a reachable IP address. If there is no valid IP,
    /// a default constructed QHostAddress is returned.
    QHostAddress getFirstIP() const;

    bool isIPValid() const;
    uint16_t getPort() const;

    /// @brief LookUpCallback
    /// The argument indicates whether a lookup was successful.
    typedef std::function<void(bool)> LookUpCallback;

    /// Looks up the network address if the address is a domain name.
    /// The callback is invoked whenever the operation is finished.
    void lookUp(LookUpCallback);

    /// @brief blockingLookUp
    /// @return Whether the DNS lookup was successful
    bool blockingLookUp();

    void setAddress(const std::string &);
    void setIPAddress(const QHostAddress &);
    void setPort(uint16_t);

    enum ATYP { IPV4 = 1, IPV6 = 4, HOST = 3 };

    ATYP addressType() const;

    std::string toString() const;

    inline bool operator< (const Address &o) const {
        return this->data < o.data;
    }

    inline bool operator== (const Address &o) const {
        return this->data == o.data;
    }

    friend inline QDataStream &operator<< (QDataStream &os,
        const Address &addr) {
        return os << QString::fromStdString(addr.toString());
    }

    friend inline QDebug &operator<< (QDebug &os, const Address &addr) {
        return os << QString::fromStdString(addr.toString());
    }

private:
    // first: address string; second: port
    std::pair<std::string, uint16_t> data;
    std::vector<QHostAddress> ipAddrList;
    std::shared_ptr<DnsLookup> dns;
};

}

#endif // ADDRESS_H
