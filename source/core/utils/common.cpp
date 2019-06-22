#include <mutex>
#include <random>
#include <sstream>
#include <vector>

#include <QHostInfo>
#include <QtEndian>

#include "utils/common.h"
#include "network/address.h"

namespace {
std::vector<QHostAddress> bannedAddresses;
std::mutex bannedAddressMutex;
} // namespace

namespace  QSS {

const uint8_t Common::ADDRESS_MASK = 0b00001111; // 0xf

const char *Common::version()
{
    return SSQT_RELEASE_VERSION;
}

// pack a shadowsocks header
std::string Common::packAddress(const Address &addr)
{
    std::string portNs(2, '\0');
    qToBigEndian(addr.getPort(), reinterpret_cast<uchar *>(&portNs[0]));
    const Address::ATYP type = addr.addressType();
    std::string addrBin;

    if(type == Address::HOST) {
        const std::string &addressString = addr.getAddress();
        // can't be longer than 255
        addrBin = static_cast<char>(addressString.length()) + addressString;
    } else if(type == Address::IPV4) {
        uint32_t ipv4Address = qToBigEndian(addr.getFirstIP().toIPv4Address());
        addrBin = std::string(reinterpret_cast<char *>(&ipv4Address), 4);
    } else {
        // Q_IPV6ADDR is a 16-unsigned-char struct (big endian)
        Q_IPV6ADDR ipv6Address = addr.getFirstIP().toIPv6Address();
        addrBin = std::string(reinterpret_cast<char *>(ipv6Address.c), 16);
    }

    char typeChar = static_cast<char>(type);
    return typeChar + addrBin + portNs;
}

std::string Common::packAddress(const QHostAddress &addr, const uint16_t &port)
{
    std::string portNs(2, '\0');
    qToBigEndian(port, reinterpret_cast<uchar *>(&portNs[0]));
    char typeChar;
    std::string addrBin;

    if(addr.protocol() == QAbstractSocket::IPv4Protocol) {
        uint32_t ipv4Address = qToBigEndian(addr.toIPv4Address());
        typeChar = static_cast<char>(Address::IPV4);
        addrBin = std::string(reinterpret_cast<char *>(&ipv4Address), 4);
    } else {
        typeChar = static_cast<char>(Address::IPV6);
        Q_IPV6ADDR ipv6Address = addr.toIPv6Address();
        addrBin = std::string(reinterpret_cast<char *>(ipv6Address.c), 16);
    }

    return typeChar + addrBin + portNs;
}

void Common::parseHeader(const std::string &data,
    Address &dest, int &header_length)
{
    header_length = 0;
    char atyp = data[0];
    int addrtype = static_cast<int>(atyp & ADDRESS_MASK);

    if(addrtype == Address::HOST) {
        if(data.length() > 2) {
            uint8_t addrlen = static_cast<uint8_t>(data[1]);

            if(data.size() >= static_cast<size_t>(2 + addrlen)) {
                dest.setPort(qFromBigEndian(*reinterpret_cast<const uint16_t *>
                    (data.data() + 2 + addrlen))
                );
                dest.setAddress(data.substr(2, addrlen));
                header_length = 4 + addrlen;
            }
        }
    } else if(addrtype == Address::IPV4) {
        if(data.length() >= 7) {
            QHostAddress addr(qFromBigEndian(*reinterpret_cast<const uint32_t *>
                (data.data() + 1))
            );

            if(!addr.isNull()) {
                header_length = 7;
                dest.setIPAddress(addr);
                dest.setPort(qFromBigEndian(*reinterpret_cast<const uint16_t *>
                    (data.data() + 5))
                );
            }
        }
    } else if(addrtype == Address::IPV6) {
        if(data.length() >= 19) {
            Q_IPV6ADDR ipv6_addr;
            memcpy(ipv6_addr.c, data.data() + 1, 16);
            QHostAddress addr(ipv6_addr);

            if(!addr.isNull()) {
                header_length = 19;
                dest.setIPAddress(addr);
                dest.setPort(qFromBigEndian(*reinterpret_cast<const uint16_t *>
                    (data.data() + 17))
                );
            }
        }
    }
}

int Common::randomNumber(int max, int min)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> dis(min, max - 1);
    return dis(engine);
}

void Common::exclusive_or(unsigned char *ks, const unsigned char *in,
    unsigned char *out, uint32_t length)
{
    unsigned char *end_ks = ks + length;

    do {
        *out = *in ^ *ks;
        ++out;
        ++in;
        ++ks;
    } while(ks < end_ks);
}

void Common::banAddress(const QHostAddress &addr)
{
    bannedAddressMutex.lock();
    bannedAddresses.push_back(addr);
    bannedAddressMutex.unlock();
}

bool Common::isAddressBanned(const QHostAddress &addr)
{
    bannedAddressMutex.lock();
    bool banned = (bannedAddresses.end() !=
        std::find(bannedAddresses.begin(), bannedAddresses.end(), addr));
    bannedAddressMutex.unlock();
    return banned;
}

std::string Common::stringFromHex(const std::string &hex)
{
    QByteArray res = QByteArray::fromHex(QByteArray(hex.data(), hex.length()));
    return std::string(res.data(), res.length());
}

} // namespace QSS
