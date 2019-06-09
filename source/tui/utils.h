#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>
#include <QStringList>

struct Utils {
    // test data encrypt/decrypt speed. print result to terminal
    static void testSpeed(const std::string &method, uint32_t data_size_mb);
    static void testSpeed(uint32_t data_size_mb); // test all methods

    /// @brief messageHandler handles Qt messages/logs
    ///
    /// @param type
    /// @param context
    /// @param msg
    static void messageHandler(QtMsgType type,
        const QMessageLogContext &context, const QString &msg);

    enum class LogLevel {
        DEBUG, INFO, WARN, ERROR, FATAL
    };
    static LogLevel logLevel;
};

#endif // UTILS_H
