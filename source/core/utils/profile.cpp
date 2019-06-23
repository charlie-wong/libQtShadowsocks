#include <QByteArray>
#include <stdexcept>

#include "profile.h"
#include "config.generated.h"

namespace QSS {

struct ProfilePrivate {
    bool debug = false;
    bool httpProxy = false;
    std::string pluginExec;
    std::string pluginOpts;
};

Profile::Profile() : m_work_mode(WorkMode::CLIENT)
    , m_local_port(DEFAULT_CLIENT_PORT)
    , m_local_addr(DEFAULT_CLIENT_ADDR)
    , m_server_port(DEFAULT_SERVER_PORT)
    , m_server_addr(DEFAULT_SERVER_ADDR)
    , m_method(DEFAULT_ALGORITHM)
    , m_password(DEFAULT_PASSWORD)
    , m_timeout(DEFAULT_SOCKET_TIMEOUT)
    , m_private(new ProfilePrivate())

{
    // Nothing Todo
}

Profile::Profile(const Profile &b) : m_work_mode(WorkMode::CLIENT)
    , m_local_port(b.m_local_port)
    , m_local_addr(b.m_local_addr)
    , m_server_port(b.m_server_port)
    , m_server_addr(b.m_server_addr)
    , m_method(b.m_method)
    , m_password(b.m_password)
    , m_timeout(b.m_timeout)
    , m_name(b.m_name)
    , m_private(new ProfilePrivate(*b.m_private))
{
    // Nothing Todo
}

Profile::Profile(Profile &&b) : m_work_mode(WorkMode::CLIENT)
    , m_local_port(std::move(b.m_local_port))
    , m_local_addr(std::move(b.m_local_addr))
    , m_server_port(std::move(b.m_server_port))
    , m_server_addr(std::move(b.m_server_addr))
    , m_method(std::move(b.m_method))
    , m_password(std::move(b.m_password))
    , m_timeout(std::move(b.m_timeout))
    , m_name(std::move(b.m_name))
    , m_private(std::move(b.m_private))
{
    // Nothing Todo
}

Profile::~Profile()
{
    // Nothing Todo
}

Profile::WorkMode Profile::getWorkMode(void) {
    return m_work_mode;
}

void Profile::setWorkMode(WorkMode mode) {
    m_work_mode = mode;
}

const std::string &Profile::name() const
{
    return m_name;
}

const std::string &Profile::method() const
{
    return m_method;
}

const std::string &Profile::password() const
{
    return m_password;
}

const std::string &Profile::serverAddress() const
{
    return m_server_addr;
}

const std::string &Profile::localAddress() const
{
    return m_local_addr;
}

const std::string &Profile::pluginExec() const
{
    return m_private->pluginExec;
}

const std::string &Profile::pluginOpts() const
{
    return m_private->pluginOpts;
}

uint16_t Profile::serverPort() const
{
    return m_server_port;
}

uint16_t Profile::localPort() const
{
    return m_local_port;
}

int Profile::timeout() const
{
    return m_timeout;
}

bool Profile::debug() const
{
    return m_private->debug;
}

bool Profile::httpProxy() const
{
    return m_private->httpProxy;
}

bool Profile::isValid() const
{
    return !method().empty() && !password().empty() && !serverAddress().empty();
}

bool Profile::hasPlugin() const
{
    return !m_private->pluginExec.empty();
}

void Profile::setName(const std::string &name)
{
    m_name = name;
}

void Profile::setMethod(const std::string &method)
{
    m_method = method;
}

void Profile::setPassword(const std::string &password)
{
    m_password = password;
}

void Profile::setServerAddress(const std::string &server)
{
    m_server_addr = server;
}

void Profile::setLocalAddress(const std::string &local)
{
    m_local_addr = local;
}

void Profile::setServerPort(uint16_t p)
{
    m_server_port = p;
}

void Profile::setLocalPort(uint16_t p)
{
    m_local_port = p;
}

void Profile::setSocketTimeout(int t)
{
    m_timeout = t;
}

void Profile::setHttpProxy(bool e)
{
    m_private->httpProxy = e;
}

void Profile::enableDebug()
{
    m_private->debug = true;
}

void Profile::disableDebug()
{
    m_private->debug = false;
}

void Profile::setPlugin(std::string exec, std::string opts)
{
    m_private->pluginExec = std::move(exec);
    m_private->pluginOpts = std::move(opts);
}

void Profile::unsetPlugin()
{
    m_private->pluginExec.clear();
    m_private->pluginOpts.clear();
}

Profile Profile::fromUri(const std::string &ssUri)
{
    if(ssUri.length() < 5) {
        throw std::invalid_argument("SS URI is too short");
    }

    Profile result;
    // remove the prefix "ss://" from uri
    std::string uri(ssUri.data() + 5, ssUri.length() - 5);
    size_t hashPos = uri.find_last_of('#');

    if(hashPos != std::string::npos) {
        // Get the name/remark
        result.setName(uri.substr(hashPos + 1));
        uri.erase(hashPos);
    }

    size_t pluginPos = uri.find_first_of('/');

    if(pluginPos != std::string::npos) {
        /// @todo: support plugins. For now, just ignore them
        uri.erase(pluginPos);
    }

    size_t atPos = uri.find_first_of('@');

    if(atPos == std::string::npos) {
        // Old URI scheme
        std::string decoded(QByteArray::fromBase64(
            QByteArray(uri.data(), uri.length()),
            QByteArray::Base64Option::OmitTrailingEquals).data()
        );
        size_t colonPos = decoded.find_first_of(':');

        if(colonPos == std::string::npos) {
            throw std::invalid_argument(
                "Can't find the colon separator between method and password"
            );
        }

        std::string method = decoded.substr(0, colonPos);
        result.setMethod(method);
        decoded.erase(0, colonPos + 1);
        atPos = decoded.find_last_of('@');

        if(atPos == std::string::npos) {
            throw std::invalid_argument(
                "Can't find the at separator between password and hostname"
            );
        }

        result.setPassword(decoded.substr(0, atPos));
        decoded.erase(0, atPos + 1);
        colonPos = decoded.find_last_of(':');

        if(colonPos == std::string::npos) {
            throw std::invalid_argument(
                "Can't find the colon separator between hostname and port"
            );
        }

        result.setServerAddress(decoded.substr(0, colonPos));
        result.setServerPort(std::stoi(decoded.substr(colonPos + 1)));
    } else {
        // SIP002 URI scheme
        std::string userInfo(QByteArray::fromBase64(
            QByteArray(uri.data(), atPos),
            QByteArray::Base64Option::Base64UrlEncoding).data()
        );
        size_t userInfoSp = userInfo.find_first_of(':');

        if(userInfoSp == std::string::npos) {
            throw std::invalid_argument(
                "Can't find the colon separator between method and password"
            );
        }

        std::string method = userInfo.substr(0, userInfoSp);
        result.setMethod(method);
        result.setPassword(userInfo.substr(userInfoSp + 1));
        uri.erase(0, atPos + 1);
        size_t hostSpPos = uri.find_last_of(':');

        if(hostSpPos == std::string::npos) {
            throw std::invalid_argument(
                "Can't find the colon separator between hostname and port"
            );
        }

        result.setServerAddress(uri.substr(0, hostSpPos));
        result.setServerPort(std::stoi(uri.substr(hostSpPos + 1)));
    }

    return result;
}

std::string Profile::toUri() const
{
    std::string ssUri = method() + ":" + password()
        + "@" + serverAddress() + ":" + std::to_string(serverPort());
    QByteArray uri = QByteArray(ssUri.data())
        .toBase64(QByteArray::Base64Option::OmitTrailingEquals);
    uri.prepend("ss://");
    uri.append("#");
    uri.append(m_name.data(), m_name.length());
    return std::string(uri.data(), uri.length());
}

std::string Profile::toUriSip002() const
{
    std::string plainUserInfo = method() + ":" + password();
    std::string userinfo(QByteArray(plainUserInfo.data()).toBase64(
        QByteArray::Base64Option::Base64UrlEncoding).data()
    );
    return "ss://" + userinfo + "@" + serverAddress()
        + ":" + std::to_string(serverPort()) + "#" + name();
}

}  // namespace QSS
