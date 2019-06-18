#ifndef TUI_UTILS_H
#define TUI_UTILS_H

#include <QtGlobal>
#include <QStringList>

struct Config {
    enum class LogLevel {
        DEBUG, INFO, WARN, ERROR, FATAL
    };

    static void setLogLevel(const QString &logLevelName);

    // test data encrypt/decrypt speed. print result to terminal
    static void testSpeed(const std::string &method, uint32_t data_size_mb);
    static void testSpeed(uint32_t data_size_mb); // test all methods

    // handles Qt log messages
    static void logMsgHandler(QtMsgType type,
        const QMessageLogContext &context, const QString &msg
    );
};

#endif // TUI_UTILS_H
