#ifndef CONFIG_CONFIGJSON_H
#define CONFIG_CONFIGJSON_H

#include <QtShadowsocks>

bool configJsonParser(const QString &file, QSS::Profile &profile);

#endif // CONFIG_CONFIGJSON_H
