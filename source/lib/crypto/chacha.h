// This is partly ported from Botan::ChaCha

#ifndef CHACHA_H
#define CHACHA_H

#include <string>
#include <vector>
#include "util/export.h"

namespace QSS {

class QSS_EXPORT ChaCha
{
public:
    /*
     * Key length must be 32 (16 is dropped)
     * IV length must be 8 or 12
     */
    ChaCha(const std::string &_key,
           const std::string &_iv);

    ChaCha(const ChaCha &) = delete;

    //encrypt (or decrypt, same process for ChaCha algorithm) a byte array.
    std::string update(const uint8_t *input, size_t length);
    std::string update(const std::string &input);

private:
    std::vector<uint32_t> m_state;
    std::vector<unsigned char> m_buffer;
    uint32_t m_position;

    void chacha();
    void setIV(const std::string &_iv);
};

}

#endif // CHACHA_H
