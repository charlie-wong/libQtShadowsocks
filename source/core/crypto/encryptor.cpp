#include "encryptor.h"
#include <QDebug>
#include <QtEndian>

#define AEAD_CHUNK_SIZE_LEN     2
#define AEAD_CHUNK_SIZE_MASK    0x3FFF

namespace {

std::string evpBytesToKey(const QSS::Cipher::CipherInfo &cipher,
    const std::string &password)
{
    int i = 0;
    std::string data;
    std::vector<std::string> m;

    while(m.size() < static_cast<size_t>(cipher.keyLen + cipher.ivLen)) {
        if(i == 0) {
            data = password;
        } else {
            data = m[i - 1] + password;
        }

        m.push_back(QSS::Cipher::md5Hash(data));
        ++i;
    }

    std::string ms;
    std::for_each(m.begin(), m.end(), [&ms](const std::string & bytes) {
        ms += bytes;
    });
    return ms.substr(0, cipher.keyLen);
}

}  // namespace

namespace  QSS {
Encryptor::Encryptor(std::string method, const std::string &password) :
    m_method(std::move(method))
    , m_cipher(Cipher::cipherInfoMap.at(m_method))
    , m_master_key(evpBytesToKey(m_cipher, password))
    , m_incomplete_length(0)
{
    // Nothing Todo
}

void Encryptor::reset()
{
    m_en_cipher.reset();
    m_de_cipher.reset();
    m_incomplete_chunk.clear();
    m_incomplete_length = 0;
}

void Encryptor::initEncipher(std::string *header)
{
    std::string key;
    std::string iv = Cipher::randomIv(m_method);

    if(m_cipher.type == Cipher::CipherType::AEAD) {
        const std::string salt = Cipher::randomIv(m_cipher.saltLen);
        key = Cipher::deriveAeadSubkey(m_cipher.keyLen, m_master_key, salt);
        *header = salt;
    } else {
        key = m_master_key;
        *header = iv;
    }

    m_en_cipher = std::make_unique<QSS::Cipher>(
        m_method, std::move(key), std::move(iv), true
    );
}

void Encryptor::initDecipher(const char *data, size_t length, size_t *offset)
{
    std::string key, iv;

    if(m_cipher.type == Cipher::CipherType::AEAD) {
        iv = std::string(m_cipher.ivLen, static_cast<char>(0));

        if(length < static_cast<size_t>(m_cipher.saltLen)) {
            throw std::length_error(
                "Data chunk is too small to initialise an AEAD decipher"
            );
        }

        key = Cipher::deriveAeadSubkey(m_cipher.keyLen, m_master_key,
            std::string(data, m_cipher.saltLen)
        );
        *offset = m_cipher.saltLen;
    } else {
        if(length < static_cast<size_t>(m_cipher.ivLen)) {
            throw std::length_error(
                "Data chunk is too small to initialise a stream decipher"
            );
        }

        iv = std::string(data, m_cipher.ivLen);
        key = m_master_key;
        *offset = m_cipher.ivLen;
    }

    m_de_cipher = std::make_unique<QSS::Cipher>(
        m_method, std::move(key), std::move(iv), false
    );
}

std::string Encryptor::encrypt(const std::string &in)
{
    return encrypt(reinterpret_cast<const uint8_t *>(in.data()), in.length());
}

std::string Encryptor::encrypt(const uint8_t *data, size_t length)
{
    if(length <= 0) {
        return std::string();
    }

    std::string header;

    if(!m_en_cipher) {
        initEncipher(&header);
    }

    std::string encrypted;

    if(m_cipher.type == Cipher::CipherType::AEAD) {
        uint16_t inLen =
            length > AEAD_CHUNK_SIZE_MASK ? AEAD_CHUNK_SIZE_MASK : length;
        std::string rawLength(AEAD_CHUNK_SIZE_LEN, static_cast<char>(0));
        qToBigEndian(inLen, reinterpret_cast<uint8_t *>(&rawLength[0]));
        std::string encLength = m_en_cipher->update(rawLength); // length + tag
        m_en_cipher->incrementIv();
        std::string encPayload = m_en_cipher->update(data, inLen); // payload + tag
        m_en_cipher->incrementIv();
        encrypted = encLength + encPayload;

        if(inLen < length) {
            // Append the remaining part recursively if there is any
            encrypted += encrypt(data + inLen, length - inLen);
        }
    } else {
        encrypted = m_en_cipher->update(data, length);
    }

    return header + encrypted;
}

std::string Encryptor::decrypt(const std::string &data)
{
    return decrypt(reinterpret_cast<const uint8_t *>(data.data()),
        data.length()
    );
}

std::string Encryptor::decrypt(const uint8_t *data, size_t length)
{
    if(length <= 0) {
        return std::string();
    }

    std::string out;

    if(!m_de_cipher) {
        size_t headerLength = 0;
        initDecipher(reinterpret_cast<const char *>(data),
            length, &headerLength
        );
        data += headerLength;
        length -= headerLength;
    }

    if(m_cipher.type == Cipher::CipherType::AEAD) {
        // Concatenate the data with incomplete chunk (if it exists)
        std::string chunk = m_incomplete_chunk + std::string(
            reinterpret_cast<const char *>(data), length
        );
        data = reinterpret_cast<const uint8_t *>(chunk.data());
        length = chunk.length();
        const uint8_t *dataEnd = data + length;
        uint16_t payloadLength = 0;

        if(m_incomplete_length != 0u) {
            // The payload length is already known
            payloadLength = m_incomplete_length;
            m_incomplete_length = 0u;
            m_incomplete_chunk.clear();
        } else {
            if(dataEnd - data < AEAD_CHUNK_SIZE_LEN + m_cipher.tagLen) {
                qDebug("AEAD data chunk is incomplete (too small for length)");
                m_incomplete_chunk = std::string(
                    reinterpret_cast<const char *>(data), dataEnd - data
                );
                return std::string();
            }

            std::string decLength = m_de_cipher->update(data,
                AEAD_CHUNK_SIZE_LEN + m_cipher.tagLen
            );
            m_de_cipher->incrementIv();
            data += (AEAD_CHUNK_SIZE_LEN + m_cipher.tagLen);
            payloadLength = qFromBigEndian(*reinterpret_cast<const uint16_t *>
                (decLength.data())) & AEAD_CHUNK_SIZE_MASK;

            if(payloadLength == 0) {
                throw std::length_error("AEAD data chunk length is invalid");
            }
        }

        if(dataEnd - data < payloadLength + m_cipher.tagLen) {
            qDebug("AEAD data chunk is incomplete (too small for payload)");
            m_incomplete_chunk = std::string(
                reinterpret_cast<const char *>(data), dataEnd - data
            );
            m_incomplete_length = payloadLength;
            return std::string();
        }

        out = m_de_cipher->update(data, payloadLength + m_cipher.tagLen);
        m_de_cipher->incrementIv();
        data += (payloadLength + m_cipher.tagLen);

        if(dataEnd > data) {
            // Append remaining decrypted chunks recursively if there is any
            out += decrypt(data, dataEnd - data);
        }
    } else {
        out = m_de_cipher->update(data, length);
    }

    return out;
}

std::string Encryptor::encryptAll(const std::string &in)
{
    return encryptAll(reinterpret_cast<const uint8_t *>(in.data()), in.length());
}

std::string Encryptor::encryptAll(const uint8_t *data, size_t length)
{
    std::string header;
    initEncipher(&header);
    return header + m_en_cipher->update(data, length);
}

std::string Encryptor::decryptAll(const std::string &data)
{
    return decryptAll(reinterpret_cast<const uint8_t *>(data.data()),
        data.length()
    );
}

std::string Encryptor::decryptAll(const uint8_t *data, size_t length)
{
    size_t headerLength = 0; // IV or salt length
    initDecipher(reinterpret_cast<const char *>(data), length, &headerLength);
    data += headerLength;
    length -= headerLength;
    return m_de_cipher->update(data, length);
}

} // namespace QSS
