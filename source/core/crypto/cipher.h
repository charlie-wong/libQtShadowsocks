// Communicate with lower-level encrytion library.
//
// Seperated from Encryptor enables us to change low-level library easier.
// If there is a modification associated with encryption/decryption, it's
// this class that needs changes instead of messing up lots of classes.

#ifndef CRYPTO_CIPHER_H
#define CRYPTO_CIPHER_H

#include <array>
#include <memory>
#include <unordered_map>

#include "rc4.h"
#include "chacha.h"

namespace Botan {
class KDF;
class Pipe;
class Keyed_Filter;
}

namespace QSS {

class Cipher {
public:
    ~Cipher();
    Cipher(Cipher &&) = default;
    Cipher(const Cipher &) = delete;

    /// @brief Cipher
    ///
    /// @param method   The cipher method name (in Shadowsocks convention)
    /// @param key      The secret key (dervied per-session for AEAD)
    /// @param iv       The initialiser vector, aka nonce
    /// @param encrypt  Whether the operation is to encrypt, otherwise it's to decrypt
    Cipher(const std::string &method, std::string key,
        std::string iv, bool encrypt
    );

    std::string update(const std::string &data);
    std::string update(const uint8_t *data, size_t length);

    /// @brief incrementIv Increments the current nonce by 1
    ///
    /// This is required by Shadowsocks AEAD operation after
    /// each encryption/decryption
    void incrementIv();

    enum CipherType { STREAM, AEAD, NONE };

    struct CipherInfo {
        std::string internalName; // internal implementation name in Botan
        int keyLen;
        int ivLen;
        CipherType type;
        int saltLen; // only for AEAD
        int tagLen; // only for AEAD
    };

    // The key of this map is the encryption method (shadowsocks convention)
    static const std::unordered_map<std::string, CipherInfo> cipherInfoMap;

    // The label/info string used for key derivation function
    static const std::string kdfLabel;

    /// @brief Generates a vector of random characters of given length
    ///
    /// @param length
    ///
    /// @return
    static std::string randomIv(int length);

    /// @brief An overloaded function to generate randomised
    /// IV for given cipher method.
    ///
    /// For AEAD ciphers, this method returns all zeros
    ///
    /// @param method The Shadowsocks cipher method name
    ///
    /// @return
    static std::string randomIv(const std::string &method);

    static std::string md5Hash(const std::string &in);

    /// @brief
    ///
    /// @param method The cipher method name in Shadowsocks convention
    ///
    /// @return True if it's supported, false otherwise
    static bool isSupported(const std::string &method);

    static std::vector<std::string> supportedMethods();

    static std::string deriveAeadSubkey(
        size_t length, const std::string &masterKey, const std::string &salt
    );

private:
    Botan::Keyed_Filter *m_filter;
    std::unique_ptr<Botan::Pipe> m_pipe;
    std::unique_ptr<RC4> m_rc4;
    std::unique_ptr<ChaCha> m_chacha;
    const std::string m_key; // preshared key
    std::string m_iv; // nonce
    const CipherInfo m_cipher;
};

} // namespace QSS

#endif // CRYPTO_CIPHER_H
