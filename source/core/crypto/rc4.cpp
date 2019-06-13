#include "rc4.h"
#include "cipher.h"
#include "util/common.h"

using namespace QSS;

RC4::RC4(const std::string &_key, const std::string &_iv) :
    position(0), x(0), y(0)
{
    Q_ASSERT(!_iv.empty());
    state.resize(256);
    buffer.resize(4096);// 4096 is the "BOTAN_DEFAULT_BUFFER_SIZE"
    std::string realKey = Cipher::md5Hash(_key + _iv);
    realKey.resize(_key.size());
    const unsigned char *key =
        reinterpret_cast<const unsigned char *>(realKey.data());

    for(uint32_t i = 0; i < 256; ++i) {
        state[i] = static_cast<unsigned char>(i);
    }

    for(uint32_t i = 0, state_index = 0; i < 256; ++i) {
        state_index =
            (state_index + key[i % realKey.length()] + state[i]) % 256;
        std::swap(state[i], state[state_index]);
    }

    generate();
}

#define delta_value   (4096 - position) // 4096 == buffer.size()
std::string RC4::update(const uint8_t *in, size_t length)
{
    std::string output;
    output.resize(length);
    unsigned char *out =
        reinterpret_cast<unsigned char *>(&output[0]);

    for(uint16_t delta = delta_value; length >= delta; delta = delta_value) {
        Common::exclusive_or(buffer.data() + position, in, out, delta);
        length -= delta;
        in += delta;
        out += delta;
        generate();
    }

    Common::exclusive_or(buffer.data() + position, in, out, length);
    position += length;
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

    for(int i = 0; i < buffer.size(); i += 4) {
        sx = state[x + 1];
        y = (y + sx) % 256;
        sy = state[y];
        state[x + 1] = sy;
        state[y] = sx;
        buffer[i] = state[(sx + sy) % 256];
        sx = state[x + 2];
        y = (y + sx) % 256;
        sy = state[y];
        state[x + 2] = sy;
        state[y] = sx;
        buffer[i + 1] = state[(sx + sy) % 256];
        sx = state[x + 3];
        y = (y + sx) % 256;
        sy = state[y];
        state[x + 3] = sy;
        state[y] = sx;
        buffer[i + 2] = state[(sx + sy) % 256];
        x = (x + 4) % 256;
        sx = state[x];
        y = (y + sx) % 256;
        sy = state[y];
        state[x] = sy;
        state[y] = sx;
        buffer[i + 3] = state[(sx + sy) % 256];
    }

    position = 0;
}