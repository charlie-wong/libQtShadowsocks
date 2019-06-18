// Somehow, the Botan::ARC4 will cause crashes on 64-bit platforms with
// unaligned memory. Therefore, I reimplemented RC4 here to get around
// the crashes by not using unaligned memory xor.

#ifndef CRYPTO_RC4_H
#define CRYPTO_RC4_H

#include <vector>
#include <string>

namespace QSS {

class RC4 {
public:
    // non-skip
    // This class implements so-called RC4-MD5 cipher instead
    // of original RC4 _iv is not allowed to be empty!
    RC4(const std::string &_key, const std::string &_iv);

    RC4(const RC4 &) = delete;

    std::string update(const uint8_t *data, size_t length);
    std::string update(const std::string &input);

private:
    void generate();

    unsigned char x;
    unsigned char y;
    uint32_t position;
    std::vector<unsigned char> state;
    std::vector<unsigned char> buffer;
};

} // namespace QSS

#endif // CRYPTO_RC4_H
