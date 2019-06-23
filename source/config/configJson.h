#ifndef CONFIG_CONFIGJSON_H
#define CONFIG_CONFIGJSON_H

#include <QtShadowsocks>

bool configJsonDefault(const QString &configJson, bool clientMode);
bool configJsonApply(const QString &file, QSS::Profile &profile);

#endif // CONFIG_CONFIGJSON_H
