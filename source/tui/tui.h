#ifndef TUI_TUI_H
#define TUI_TUI_H

#include <QtShadowsocks>

class QtSsTui {
public:
    QtSsTui();
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

    bool start(void);

    enum WorkMode { SERVER, CLIENT };

    void setAutoBan(bool ban);
    void setHttpMode(bool http);
    void setWorkMode(WorkMode mode);
    const std::string &getMethod() const;
private:
    bool headerTest();

    bool autoBan;
    WorkMode m_work_mode;
    QSS::Profile m_profile;
    std::unique_ptr<QSS::Controller> m_ctrl;
    std::unique_ptr<QSS::Connectivity> m_tester;
};

#endif // TUI_TUI_H
