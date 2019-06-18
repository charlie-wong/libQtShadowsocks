// High-level API to encrypt/decrypt data that send to or receive from
// another shadowsocks side.
// This class shouldn't contain too many detailed (de)encrypt functions.
// Instead, it should use Cipher class as much as possible.
// The only exception for this rule is the deprecated TABLE method.

#ifndef CRYPTO_ENCRYPTOR_H
#define CRYPTO_ENCRYPTOR_H

#include <memory>

#include "cipher.h"

namespace QSS {

class Encryptor {
public:
    /// @brief Encryptor
    ///
    /// @param method The encryption method in Shadowsocks convention
    /// @param password The preshared password
    Encryptor(std::string method, const std::string &password);

    Encryptor(const Encryptor &) = delete;

    /// @brief decrypt Decrypts encrypted shadowsocks TCP packets
    ///
    /// @return Decrypted data
    std::string decrypt(const std::string &);

    /// @brief decrypt Decrypts encrypted shadowsocks TCP packets
    ///
    /// @return Decrypted data
    std::string decrypt(const uint8_t *data, size_t length);

    /// @brief encrypt Encrypts plain text in TCP sessions
    ///
    /// @return Encrypted data
    std::string encrypt(const std::string &);

    /// @brief encrypt Encrypts plain text in TCP sessions
    ///
    /// @return Encrypted data
    std::string encrypt(const uint8_t *data, size_t length);

    // decryptAll and encryptAll are the counterpart for UDP packets
    std::string decryptAll(const std::string &);
    std::string decryptAll(const uint8_t *data, size_t length);

    std::string encryptAll(const std::string &);
    std::string encryptAll(const uint8_t *data, size_t length);

    /// @brief reset Resets this Encryptor to initial state
    void reset();

private:
    std::string m_method;
    const Cipher::CipherInfo cipherInfo;
    std::string masterKey;
    std::string incompleteChunk;
    uint16_t incompleteLength;

    void initEncipher(std::string *header);
    void initDecipher(const char *data, size_t length, size_t *offset);

protected:
    std::unique_ptr<Cipher> enCipher;
    std::unique_ptr<Cipher> deCipher;
};

} // namespace QSS

#endif // CRYPTO_ENCRYPTOR_H
