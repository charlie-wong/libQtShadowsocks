#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

#include "connection.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog {
    Q_OBJECT

public:
    ~EditDialog();
    explicit EditDialog(Connection *_connection, QWidget *parent = 0);

private:
    Ui::EditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // EDITDIALOG_H
