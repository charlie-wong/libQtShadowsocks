#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QSettings>

#include "connection.h"
#include "connectiontablemodel.h"

class ConfigHelper : public QObject {
    Q_OBJECT

public:
    // Construct a ConfigHelper object using specified configuration
    // file, this constructor will call readGeneralSettings()
    explicit ConfigHelper(const QString &config, QObject *parent = nullptr);

    // Call read() function to read all connection profiles into specified
    // ConnectionTableModel. This function also calls readGeneralSettings().
    void read(ConnectionTableModel *model);

    // only reads [General] settings and store them into member variables.
    void readGeneralSettings();

    void save(const ConnectionTableModel &model);

    Connection *configJsonToConnection(const QString &file);
    void importGuiConfigJson(ConnectionTableModel *model, const QString &file);
    void exportGuiConfigJson(const ConnectionTableModel &model, const QString &file);

    // create or delete start up item
    void setStartAtLogin();
    // start those connections marked as auto-start
    void startAllAutoStart(const ConnectionTableModel &model);

    // some functions used to communicate with SettingsDialog
    int  getToolbarStyle() const;
    bool isHideWindowOnStartup() const;
    bool isStartAtLogin() const;
    bool isOnlyOneInstance() const;
    bool isShowToolbar() const;
    bool isShowFilterBar() const;
    bool isNativeMenuBar() const;
    void setGeneralSettings(int ts, bool hide, bool automaticStartUp,
        bool oneInstance, bool nativeMB);
    void setMainWindowGeometry(const QByteArray &geometry);
    void setMainWindowState(const QByteArray &state);
    void setTableGeometry(const QByteArray &geometry);
    void setTableState(const QByteArray &state);

    QByteArray getMainWindowGeometry() const;
    QByteArray getMainWindowState() const;
    QByteArray getTableGeometry() const;
    QByteArray getTableState() const;

public slots:
    void setShowToolbar(bool show);
    void setShowFilterBar(bool show);

signals:
    void toolbarStyleChanged(const Qt::ToolButtonStyle);

private:
    bool isFileValid(void) const;
    QByteArray getUiState(const QString &key) const;
    void setUiState(const QString &key, const QByteArray &data);
    void checkProfileDataUsageReset(SQProfile &profile);

    QString m_configFile;
    QSettings *m_settings;

    int m_toolbarStyle;
    bool m_showToolbar;
    bool m_startOnLogin;
    bool m_isOneInstace;
    bool m_showFilterBar;
    bool m_hideOnStartup;
    bool m_useNativeMenu;
};

#endif // CONFIGHELPER_H
