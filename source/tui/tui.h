#ifndef TUI_TUI_H
#define TUI_TUI_H

#include <QtShadowsocks>

class QtSsTui {
public:
    QtSsTui();
    bool parseConfigJson(const QString &);

    void setup(const QString &remote_addr,
        const QString &remote_port,
        const QString &local_addr,
        const QString &local_port,
        const QString &password,
        const QString &method,
        const QString &timeout,
        const bool http_proxy
    );

    bool start(void);
    void setHttpMode(bool http);
    const std::string &getMethod() const;
    void setWorkMode(QSS::Profile::WorkMode mode);
private:
    bool headerTest();

    QSS::Profile m_profile;
    std::unique_ptr<QSS::Controller> m_ctrl;
    std::unique_ptr<QSS::Connectivity> m_conn;
};

#endif // TUI_TUI_H
