#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QSettings>

#include "connection.h"
#include "connectiontablemodel.h"

class ConfigHelper : public QObject {
    Q_OBJECT

public:
    // Construct a ConfigHelper object using specified configuration file
    // This constructor will call readGeneralSettings().
    explicit ConfigHelper(const QString &configuration, QObject *parent = nullptr);

    // Call read() function to read all connection profiles into specified
    // ConnectionTableModel. This function also calls readGeneralSettings().
    void read(ConnectionTableModel *model);

    // readGeneralSettings() only reads General settings and store them into
    // member variables.
    void readGeneralSettings();

    void save(const ConnectionTableModel &model);

    void importGuiConfigJson(ConnectionTableModel *model, const QString &file);

    // format is only compatible with shadowsocks-csharp (shadowsocks-windows)
    void exportGuiConfigJson(const ConnectionTableModel &model, const QString &file);

    Connection *configJsonToConnection(const QString &file);

    // start those connections marked as auto-start
    void startAllAutoStart(const ConnectionTableModel &model);

    // create or delete start up item for ShadowSocksQt-GUI
    void setStartAtLogin();

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
    void checkProfileDataUsageReset(SQProfile &profile);

    int toolbarStyle;
    bool hideWindowOnStartup;
    bool startAtLogin;
    bool onlyOneInstace;
    bool showToolbar;
    bool showFilterBar;
    bool nativeMenuBar;
    QSettings *settings;
    QString configFile;
};

#endif // CONFIGHELPER_H
