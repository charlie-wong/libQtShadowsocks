#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include <QHostAddress>

namespace QSS {

class Address;

namespace Common {

const char *version();
std::string packAddress(const Address &addr);
// this will never use ADDRTYPE_HOST because addr is an IP address
std::string packAddress(const QHostAddress &addr, const uint16_t &port);
void parseHeader(const std::string &data, Address &addr, int &length);
// generate a random number which is in the range [min, max)
int randomNumber(int max, int min = 0);
void exclusive_or(unsigned char *ks, const unsigned char *in,
    unsigned char *out, uint32_t length
);
void banAddress(const QHostAddress &addr);
bool isAddressBanned(const QHostAddress &addr);

std::string stringFromHex(const std::string &);

extern const uint8_t ADDRESS_MASK;

} // namespace Common

} // namespace QSS

#endif // UTILS_COMMON_H
