#include "rc4.h"
#include "cipher.h"
#include "utils/common.h"

using namespace QSS;

RC4::RC4(const std::string &_key, const std::string &_iv) :
    m_x(0), m_y(0), m_position(0)
{
    Q_ASSERT(!_iv.empty());
    m_state.resize(256);
    m_buffer.resize(4096);// 4096 is the "BOTAN_DEFAULT_BUFFER_SIZE"
    std::string realKey = Cipher::md5Hash(_key + _iv);
    realKey.resize(_key.size());
    const unsigned char *key =
        reinterpret_cast<const unsigned char *>(realKey.data());

    for(uint32_t i = 0; i < 256; ++i) {
        m_state[i] = static_cast<unsigned char>(i);
    }

    for(uint32_t i = 0, state_index = 0; i < 256; ++i) {
        state_index =
            (state_index + key[i % realKey.length()] + m_state[i]) % 256;
        std::swap(m_state[i], m_state[state_index]);
    }

    generate();
}

#define delta_value   (4096 - m_position) // 4096 == m_buffer.size()
std::string RC4::update(const uint8_t *in, size_t length)
{
    std::string output;
    output.resize(length);
    unsigned char *out =
        reinterpret_cast<unsigned char *>(&output[0]);

    for(uint16_t delta = delta_value; length >= delta; delta = delta_value) {
        Common::exclusive_or(m_buffer.data() + m_position, in, out, delta);
        length -= delta;
        in += delta;
        out += delta;
        generate();
    }

    Common::exclusive_or(m_buffer.data() + m_position, in, out, length);
    m_position += length;
    return output;
}
#undef delta_value

std::string RC4::update(const std::string &input)
{
    return update(reinterpret_cast<const uint8_t *>(input.data()),
        input.length()
    );
}

void RC4::generate()
{
    unsigned char sx, sy;

    for(unsigned i = 0; i < m_buffer.size(); i += 4) {
        sx = m_state[m_x + 1];
        m_y = (m_y + sx) % 256;
        sy = m_state[m_y];
        m_state[m_x + 1] = sy;
        m_state[m_y] = sx;
        m_buffer[i] = m_state[(sx + sy) % 256];
        sx = m_state[m_x + 2];
        m_y = (m_y + sx) % 256;
        sy = m_state[m_y];
        m_state[m_x + 2] = sy;
        m_state[m_y] = sx;
        m_buffer[i + 1] = m_state[(sx + sy) % 256];
        sx = m_state[m_x + 3];
        m_y = (m_y + sx) % 256;
        sy = m_state[m_y];
        m_state[m_x + 3] = sy;
        m_state[m_y] = sx;
        m_buffer[i + 2] = m_state[(sx + sy) % 256];
        m_x = (m_x + 4) % 256;
        sx = m_state[m_x];
        m_y = (m_y + sx) % 256;
        sy = m_state[m_y];
        m_state[m_x] = sy;
        m_state[m_y] = sx;
        m_buffer[i + 3] = m_state[(sx + sy) % 256];
    }

    m_position = 0;
}
