#ifndef SHAREDIALOG_H
#define SHAREDIALOG_H

#include <QDialog>

namespace Ui {
class ShareDialog;
}

class ShareDialog : public QDialog {
    Q_OBJECT

public:
    ~ShareDialog();
    explicit ShareDialog(const QByteArray &ssUrl, QWidget *parent = 0);

private:
    Ui::ShareDialog *ui;

private slots:
    void onSaveButtonClicked();
};

#endif // SHAREDIALOG_H
