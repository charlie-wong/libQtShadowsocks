#ifndef CLIENT_H
#define CLIENT_H

#include <QtShadowsocks>

class Client {
public:
    Client();
    bool readConfig(const QString &);

    void setup(const QString &remote_addr,
        const QString &remote_port,
        const QString &local_addr,
        const QString &local_port,
        const QString &password,
        const QString &method,
        const QString &timeout,
        const bool http_proxy
    );

    void setAutoBan(bool ban);
    void setHttpMode(bool http);
    const std::string &getMethod() const;
    bool start(bool serverMode = false);

private:
    std::unique_ptr<QSS::Controller> controller;
    std::unique_ptr<QSS::AddressTester> tester;
    QSS::Profile profile;
    bool autoBan;
    bool headerTest();
};

#endif // CLIENT_H
