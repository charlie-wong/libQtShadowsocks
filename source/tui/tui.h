#ifndef TUI_TUI_H
#define TUI_TUI_H

#include <QtShadowsocks>

class QtSsTui {
public:
    QtSsTui();
    bool applyConfigJson(const QString &);

    void setup(const QString &remote_addr, const QString &remote_port,
        const QString &local_addr, const QString &local_port,
        const QString &password, const QString &algorithm,
        const QString &socket_timeout, const bool http_proxy
    );

    bool start(void);
    const std::string &getMethod() const;
    void setWorkMode(QSS::Profile::WorkMode mode);
private:
    bool headerTest();

    QSS::Profile m_profile;
    std::unique_ptr<QSS::Controller> m_ctrl;
    std::unique_ptr<QSS::Connectivity> m_conn;
};

#endif // TUI_TUI_H
