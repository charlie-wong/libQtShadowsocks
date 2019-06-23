#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QCoreApplication>

#include "confighelper.h"

#define FILE_VERSION    1.0
#define SEC_General     "General"
#define SEC_UIState     "UIState"
#define SEC_Profile     "Profile"

ConfigHelper::ConfigHelper(const QString &config, QObject *parent) :
    QObject(parent), m_configFile(config)
{
    m_settings = new QSettings(m_configFile, QSettings::IniFormat, this);
    readGeneralSettings();
}

void ConfigHelper::save(const ConnectionTableModel &model)
{
    // m_settings->beginGroup(SEC_General);
    m_settings->setValue("FileVersion", QVariant(FILE_VERSION));
    m_settings->setValue("ToolbarStyle", QVariant(m_toolbarStyle));
    m_settings->setValue("HideOnStartup", QVariant(m_hideOnStartup));
    m_settings->setValue("StartAtLogin", QVariant(m_startOnLogin));
    m_settings->setValue("IsOneInstance", QVariant(m_isOneInstace));
    m_settings->setValue("ShowToolbar", QVariant(m_showToolbar));
    m_settings->setValue("ShowFilterBar", QVariant(m_showFilterBar));
    m_settings->setValue("UseNativeMenu", QVariant(m_useNativeMenu));
    // m_settings->endGroup();

    m_settings->beginWriteArray(SEC_Profile);
    for(int i = 0; i < model.rowCount(); ++i) {
        m_settings->setArrayIndex(i);
        Connection *con = model.getItem(i)->getConnection();
        QVariant value = QVariant::fromValue<SQProfile>(con->getProfile());
        m_settings->setValue("SQProfile", value);
    }
    m_settings->endArray();
}

void ConfigHelper::importGuiConfigJson(ConnectionTableModel *model,
    const QString &file)
{
    QFile jsonFile(file);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if(!jsonFile.isOpen()) {
        qWarning() << "Warning: can not open" << file;
        return;
    }

    if(!jsonFile.isReadable()) {
        qWarning() << "Warning: can not read" << file;
        return;
    }

    QJsonParseError pe;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll(), &pe);
    jsonFile.close();

    if(pe.error != QJsonParseError::NoError) {
        qWarning() << "Warning: JSON document invalid" << pe.errorString();
        return;
    }

    if(jsonDoc.isEmpty()) {
        qWarning() << "Warning: JSON document" << file << "is empty!";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray connArray = jsonObj["Connections"].toArray();
    if(connArray.isEmpty()) {
        qWarning() << "Warning: 'Connections' of" << file << "is empty.";
        return;
    }

    for(auto it = connArray.begin(); it != connArray.end(); it++) {
        SQProfile profile;
        QJsonObject json = (*it).toObject();
        profile.name = json["ProfileName"].toString();
        profile.timeout = json["Timeout"].toString().toInt();
        profile.method = json["Algorithm"].toString();
        profile.password = json["Password"].toString();
        profile.serverAddress = json["ServerAddr"].toString();
        profile.serverPort = json["ServerPort"].toString().toUShort();
        profile.localAddress = json["ClientAddr"].toString();
        profile.localPort = json["ClientPort"].toString().toUShort();
        Connection *conn = new Connection(profile, this);
        model->appendConnection(conn);
    }
}

void ConfigHelper::exportGuiConfigJson(const ConnectionTableModel &model,
    const QString &file)
{
    QFile jsonFile(file);
    jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!jsonFile.isOpen()) {
        qWarning() << "Warning: can not open" << file;
        return;
    }

    if(!jsonFile.isWritable()) {
        qWarning() << "Warning: can not write" << file;
        return;
    }

    QJsonArray conArray;

    for(int i = 0; i < model.rowCount(); ++i) {
        QJsonObject jsonObj;
        Connection *conn = model.getItem(i)->getConnection();
        jsonObj["ProfileName"] = QJsonValue(conn->profile.name);
        jsonObj["Timeout"] = QJsonValue(conn->profile.timeout);
        jsonObj["Algorithm"] = QJsonValue(conn->profile.method.toLower());
        jsonObj["Password"] = QJsonValue(conn->profile.password);
        jsonObj["ServerPort"] = QJsonValue(conn->profile.serverPort);
        jsonObj["ServerAddr"] = QJsonValue(conn->profile.serverAddress);
        jsonObj["ClientPort"] = QJsonValue(conn->profile.localPort);
        jsonObj["ClientAddr"] = QJsonValue(conn->profile.localAddress);
        conArray.append(QJsonValue(jsonObj)); // connection JSON object
    }

    QJsonObject jsonObj;
    jsonObj["Connections"] = QJsonValue(conArray);

    QJsonDocument jsonDoc(jsonObj);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}

Connection *ConfigHelper::configJsonToConnection(const QString &file)
{
    QFile jsonFile(file);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if(!jsonFile.isOpen()) {
        qCritical() << "Error: can not open" << file;
        return nullptr;
    }

    if(!jsonFile.isReadable()) {
        qCritical() << "Error: can not read" << file;
        return nullptr;
    }

    QJsonParseError pe;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll(), &pe);
    jsonFile.close();

    if(pe.error != QJsonParseError::NoError) {
        qCritical() << "Error: JSON format invalid" << pe.errorString();
        return nullptr;
    }

    if(jsonDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        return nullptr;
    }

    SQProfile profile;
    QJsonObject jsonObj = jsonDoc.object();
    profile.serverAddress = jsonObj["ServerAddr"].toString();
    profile.serverPort = jsonObj["ServerPort"].toInt();
    profile.localAddress = jsonObj["ClientAddr"].toString();
    profile.localPort = jsonObj["ClientPort"].toInt();
    profile.method = jsonObj["Algorithm"].toString();
    profile.password = jsonObj["Password"].toString();
    profile.timeout = jsonObj["Timeout"].toInt();

    return new Connection(profile, this);
}

int ConfigHelper::getToolbarStyle() const
{
    return m_toolbarStyle;
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return m_hideOnStartup;
}

bool ConfigHelper::isStartAtLogin() const
{
    return m_startOnLogin;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return m_isOneInstace;
}

bool ConfigHelper::isShowToolbar() const
{
    return m_showToolbar;
}

bool ConfigHelper::isShowFilterBar() const
{
    return m_showFilterBar;
}

bool ConfigHelper::isNativeMenuBar() const
{
    return m_useNativeMenu;
}

void ConfigHelper::setGeneralSettings(int ts, bool hide, bool sal,
    bool oneInstance, bool nativeMB)
{
    if(m_toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }

    m_toolbarStyle = ts;
    m_hideOnStartup = hide;
    m_startOnLogin = sal;
    m_isOneInstace = oneInstance;
    m_useNativeMenu = nativeMB;
}

void ConfigHelper::setShowToolbar(bool show)
{
    m_showToolbar = show;
}

void ConfigHelper::setShowFilterBar(bool show)
{
    m_showFilterBar = show;
}

bool ConfigHelper::isFileValid(void) const
{
    qreal fileVersion = m_settings->value("FileVersion").toReal();

    if(fileVersion >= FILE_VERSION) {
        return true;
    }

    return false;
}

void ConfigHelper::read(ConnectionTableModel *model)
{
    if(!isFileValid()) {
        return;
    }

    for(int i = 0; i < m_settings->beginReadArray(SEC_Profile); ++i) {
        m_settings->setArrayIndex(i);
        QVariant value = m_settings->value("SQProfile");
        SQProfile profile = value.value<SQProfile>();
        checkProfileDataUsageReset(profile);
        Connection *con = new Connection(profile, this);
        model->appendConnection(con);
    }

    m_settings->endArray();
    readGeneralSettings();
}

void ConfigHelper::readGeneralSettings()
{
    if(!isFileValid()) {
        return;
    }

    m_startOnLogin = m_settings->value("StartAtLogin").toBool();
    m_hideOnStartup = m_settings->value("HideOnStartup").toBool();
    m_showToolbar = m_settings->value("ShowToolbar", QVariant(true)).toBool();
    m_toolbarStyle = m_settings->value("ToolbarStyle", QVariant(4)).toInt();
    m_isOneInstace = m_settings->value("IsOneInstance", QVariant(true)).toBool();
    m_showFilterBar = m_settings->value("ShowFilterBar", QVariant(true)).toBool();
    m_useNativeMenu = m_settings->value("UseNativeMenu", QVariant(false)).toBool();
}

void ConfigHelper::checkProfileDataUsageReset(SQProfile &profile)
{
    QDate currentDate = QDate::currentDate();

    if(profile.nextResetDate.isNull()) {
        // invalid if the config.ini is old the
        // default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        qDebug() << "config.ini upgraded from old version";
        // we used to use sent and received
        profile.totalUsage += profile.currentUsage;
    }

    if(profile.nextResetDate <
        currentDate) {
        // not <= because that'd casue multiple reset on this day
        profile.currentUsage = 0;

        while(profile.nextResetDate <= currentDate) {
            profile.nextResetDate = profile.nextResetDate.addMonths(1);
        }
    }
}

void ConfigHelper::startAllAutoStart(const ConnectionTableModel &model)
{
    int size = model.rowCount();

    for(int i = 0; i < size; ++i) {
        Connection *con = model.getItem(i)->getConnection();

        if(con->profile.autoStart) {
            con->start();
        }
    }
}

void ConfigHelper::setStartAtLogin()
{
    QString applicationName = "ShadowSocksQt";
    QString applicationFilePath =
        QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

#if defined(Q_OS_WIN)
    QSettings win_settings(
        "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat
    );
#elif defined(Q_OS_LINUX)
    QFile file(QDir::homePath() + "/.config/autostart/shadowsocksqt.desktop");
    QString fileContent(
        "[Desktop Entry]\n"
        "Name=%1\n"
        "Exec=%2\n"
        "Type=Application\n"
        "Terminal=false\n"
        "X-GNOME-Autostart-enabled=true\n");
#elif defined(Q_OS_MAC)
    QFile file(QDir::homePath() +
        "/Library/LaunchAgents/org.shadowsocks.shadowsocksqt.launcher.plist"
    );
    QString fileContent(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN"
        "http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        "<plist version=\"1.0\">\n"
        "<dict>\n"
        "  <key>Label</key>\n"
        "  <string>org.shadowsocks.shadowsocksqt.launcher</string>\n"
        "  <key>LimitLoadToSessionType</key>\n"
        "  <string>Aqua</string>\n"
        "  <key>ProgramArguments</key>\n"
        "  <array>\n"
        "    <string>%2</string>\n"
        "  </array>\n"
        "  <key>RunAtLoad</key>\n"
        "  <true/>\n"
        "  <key>StandardErrorPath</key>\n"
        "  <string>/dev/null</string>\n"
        "  <key>StandardOutPath</key>\n"
        "  <string>/dev/null</string>\n"
        "</dict>\n"
        "</plist>\n"
    );
#else
    QFile file;
    QString fileContent;
#endif

    if(this->isStartAtLogin()) { // Create start up item
    #if defined(Q_OS_WIN)
        win_settings.setValue(applicationName, applicationFilePath);
    #else
        fileContent.replace("%1", applicationName);
        fileContent.replace("%2", applicationFilePath);

        if(file.open(QIODevice::WriteOnly)) {
            file.write(fileContent.toUtf8());
            file.close();
        }
    #endif
    } else { // Delete start up item
    #if defined(Q_OS_WIN)
        win_settings.remove(applicationName);
    #else
        if(file.exists()) {
            file.remove();
        }
    #endif
    }
}

QByteArray ConfigHelper::getUiState(const QString &key) const
{
    if(!isFileValid()) {
        return QByteArray();
    }

    m_settings->beginGroup(SEC_UIState);
    QByteArray data = m_settings->value(key).toByteArray();
    m_settings->endGroup();
    return data;
}

void ConfigHelper::setUiState(const QString &key, const QByteArray &data)
{
    m_settings->beginGroup(SEC_UIState);
    m_settings->setValue(key, QVariant(data));
    m_settings->endGroup();
}

QByteArray ConfigHelper::getMainWindowState() const
{
    return getUiState("MainWindowState");
}

void ConfigHelper::setMainWindowState(const QByteArray &state)
{
    setUiState("MainWindowState", state);
}

QByteArray ConfigHelper::getTableGeometry() const
{
    return getUiState("MainTableGeometry");
}

void ConfigHelper::setTableGeometry(const QByteArray &geometry)
{
    setUiState("MainTableGeometry", geometry);
}

QByteArray ConfigHelper::getTableState() const
{
    return getUiState("MainTableState");
}

void ConfigHelper::setTableState(const QByteArray &state)
{
    setUiState("MainTableState", state);
}

QByteArray ConfigHelper::getMainWindowGeometry() const
{
    return getUiState("MainWindowGeometry");
}

void ConfigHelper::setMainWindowGeometry(const QByteArray &geometry)
{
    setUiState("MainWindowGeometry", geometry);
}
