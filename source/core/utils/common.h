#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include <QHostAddress>

namespace QSS {

class Address;

namespace Common {

const char *version();

// generate a random number which is in the range [min, max)
int randomNumber(int max, int min=0);

void banAddress(const QHostAddress &addr);
bool isAddressBanned(const QHostAddress &addr);

std::string hexToStr(const std::string &hex);

std::string packAddress(const Address &addr);
std::string packAddress(const QHostAddress &addr, const uint16_t &port);

void parseHeader(const std::string &data, Address &addr, int &length);

void exclusive_or(unsigned char *ks, const unsigned char *in,
    unsigned char *out, uint32_t length
);

} // namespace Common

} // namespace QSS

#endif // UTILS_COMMON_H
