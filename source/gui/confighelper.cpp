#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QCoreApplication>

#include "confighelper.h"

ConfigHelper::ConfigHelper(const QString &configuration, QObject *parent) :
    QObject(parent), configFile(configuration)
{
    settings = new QSettings(configFile, QSettings::IniFormat, this);
    readGeneralSettings();
}

const QString ConfigHelper::profilePrefix = "Profile";

void ConfigHelper::save(const ConnectionTableModel &model)
{
    int size = model.rowCount();
    settings->beginWriteArray(profilePrefix);

    for(int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Connection *con = model.getItem(i)->getConnection();
        QVariant value = QVariant::fromValue<SQProfile>(con->getProfile());
        settings->setValue("SQProfile", value);
    }

    settings->endArray();
    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("StartAtLogin", QVariant(startAtLogin));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
    settings->setValue("NativeMenuBar", QVariant(nativeMenuBar));
    settings->setValue("ConfigVersion", QVariant(2.6));
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
    return toolbarStyle;
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return hideWindowOnStartup;
}

bool ConfigHelper::isStartAtLogin() const
{
    return startAtLogin;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return onlyOneInstace;
}

bool ConfigHelper::isShowToolbar() const
{
    return showToolbar;
}

bool ConfigHelper::isShowFilterBar() const
{
    return showFilterBar;
}

bool ConfigHelper::isNativeMenuBar() const
{
    return nativeMenuBar;
}

void ConfigHelper::setGeneralSettings(int ts, bool hide, bool sal,
    bool oneInstance, bool nativeMB)
{
    if(toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }

    toolbarStyle = ts;
    hideWindowOnStartup = hide;
    startAtLogin = sal;
    onlyOneInstace = oneInstance;
    nativeMenuBar = nativeMB;
}

void ConfigHelper::setShowToolbar(bool show)
{
    showToolbar = show;
}

void ConfigHelper::setShowFilterBar(bool show)
{
    showFilterBar = show;
}

void ConfigHelper::read(ConnectionTableModel *model)
{
    int size = settings->beginReadArray(profilePrefix);
    // qreal configVer = settings->value("ConfigVersion", QVariant(2.4)).toReal();

    for(int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("SQProfile");
        SQProfile profile = value.value<SQProfile>();
        checkProfileDataUsageReset(profile);
        Connection *con = new Connection(profile, this);
        model->appendConnection(con);
    }

    settings->endArray();
    readGeneralSettings();
}

void ConfigHelper::readGeneralSettings()
{
    toolbarStyle = settings->value("ToolbarStyle", QVariant(4)).toInt();
    startAtLogin = settings->value("StartAtLogin").toBool();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
    nativeMenuBar = settings->value("NativeMenuBar", QVariant(false)).toBool();
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
    QSettings settings(
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
        settings.setValue(applicationName, applicationFilePath);
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
        settings.remove(applicationName);
    #else
        if(file.exists()) {
            file.remove();
        }
    #endif
    }
}

QByteArray ConfigHelper::getMainWindowGeometry() const
{
    return settings->value("MainWindowGeometry").toByteArray();
}

QByteArray ConfigHelper::getMainWindowState() const
{
    return settings->value("MainWindowState").toByteArray();
}

QByteArray ConfigHelper::getTableGeometry() const
{
    return settings->value("MainTableGeometry").toByteArray();
}

QByteArray ConfigHelper::getTableState() const
{
    return settings->value("MainTableState").toByteArray();
}

void ConfigHelper::setMainWindowGeometry(const QByteArray &geometry)
{
    settings->setValue("MainWindowGeometry", QVariant(geometry));
}

void ConfigHelper::setMainWindowState(const QByteArray &state)
{
    settings->setValue("MainWindowState", QVariant(state));
}

void ConfigHelper::setTableGeometry(const QByteArray &geometry)
{
    settings->setValue("MainTableGeometry", QVariant(geometry));
}

void ConfigHelper::setTableState(const QByteArray &state)
{
    settings->setValue("MainTableState", QVariant(state));
}
