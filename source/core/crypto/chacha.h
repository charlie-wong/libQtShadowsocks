// This is partly ported from Botan::ChaCha

#ifndef CRYPTO_CHACHA_H
#define CRYPTO_CHACHA_H

#include <string>
#include <vector>

namespace QSS {

class ChaCha {
public:
    // Key length must be 32 (16 is dropped)
    // IV length must be 8 or 12
    ChaCha(const std::string &_key, const std::string &_iv);

    ChaCha(const ChaCha &) = delete;

    // encrypt (or decrypt, same process for ChaCha algorithm) a byte array.
    std::string update(const uint8_t *input, size_t length);
    std::string update(const std::string &input);

private:
    uint32_t m_position;
    std::vector<uint32_t> m_state;
    std::vector<unsigned char> m_buffer;

    void chacha();
    void setIV(const std::string &_iv);
};

} // namespace QSS

#endif // CRYPTO_CHACHA_H
