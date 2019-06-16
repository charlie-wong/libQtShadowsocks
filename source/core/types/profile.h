#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <cstdint>
#include <memory>

#include "utils/export.h"

namespace QSS {

struct ProfilePrivate;

class QSS_EXPORT Profile {
public:
    Profile();
    Profile(const Profile &);
    Profile(Profile &&);
    ~Profile();

    const std::string &name() const;
    const std::string &method() const;
    const std::string &password() const;
    const std::string &serverAddress() const;
    const std::string &localAddress() const;
    const std::string &pluginExec() const;
    const std::string &pluginOpts() const;
    uint16_t serverPort() const;
    uint16_t localPort() const;
    int timeout() const;
    bool httpProxy() const;
    bool debug() const;
    bool hasPlugin() const;

    /// @brief isValid Whether this profile has essential information.
    ///
    /// The validation only checks if essential fields such as method,
    /// address are set, but it doesn't verify the validity of each field.
    ///
    /// @return true or false
    bool isValid() const;

    std::string toUri() const;
    std::string toUriSip002() const;

    void setName(const std::string &name);
    void setMethod(const std::string &method);
    void setPassword(const std::string &password);
    void setServerAddress(const std::string &server);
    void setLocalAddress(const std::string &local);
    void setServerPort(uint16_t);
    void setLocalPort(uint16_t);
    void setTimeout(int);
    void setHttpProxy(bool);
    void enableDebug();
    void disableDebug();
    void setPlugin(std::string exec, std::string opts = std::string());
    void unsetPlugin();

    // Both the original schema and the SIP002 are supported by this function
    static Profile fromUri(const std::string &);

private:
    std::unique_ptr<ProfilePrivate>
    d_private; // For future extension (to keep ABI compatible)

    // Essential data are stored directly as members in this class. Don't
    // remove or change any these members, which would break ABI compatiblity
    std::string d_name;
    std::string d_method;
    std::string d_password;
    std::string d_serverAddress;
    std::string d_localAddress;
    uint16_t d_serverPort;
    uint16_t d_localPort;
    int d_timeout;
};

}
#endif // PROFILE_H
