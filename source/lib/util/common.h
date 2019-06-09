#ifndef COMMON_H
#define COMMON_H

#include <QHostAddress>
#include "export.h"

namespace QSS {

class Address;

namespace Common {

QSS_EXPORT const char *version();
QSS_EXPORT std::string packAddress(const Address &addr);
// this will never use ADDRTYPE_HOST because addr is an IP address
QSS_EXPORT std::string packAddress(const QHostAddress &addr,
    const uint16_t &port
);
QSS_EXPORT void parseHeader(const std::string &data,
    Address &addr, int &length
);
// generate a random number which is in the range [min, max)
QSS_EXPORT int randomNumber(int max, int min = 0);
QSS_EXPORT void exclusive_or(unsigned char *ks, const unsigned char *in,
    unsigned char *out, uint32_t length
);
QSS_EXPORT void banAddress(const QHostAddress &addr);
QSS_EXPORT bool isAddressBanned(const QHostAddress &addr);

QSS_EXPORT std::string stringFromHex(const std::string &);

extern const uint8_t ADDRESS_MASK;

} // namespace Common

} // namespace QSS

#endif // COMMON_H
