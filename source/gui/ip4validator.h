#ifndef IP4VALIDATOR_H
#define IP4VALIDATOR_H

#include <QValidator>

class IP4Validator : public QValidator {
public:
    IP4Validator(QObject *parent = 0);
    State validate(QString &input, int &) const;
};

#endif // IP4VALIDATOR_H
