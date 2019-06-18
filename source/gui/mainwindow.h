#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QLocalServer>
#include "connectiontablemodel.h"
#include "confighelper.h"
#include "statusnotifier.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(ConfigHelper *confHelper, QWidget *parent = 0);
    ~MainWindow();

    void startAutoStartConnections();
    bool isInstanceRunning() const;

private:
    Ui::MainWindow *ui;

    ConnectionTableModel *model;
    QSortFilterProxyModel *proxyModel;
    ConfigHelper *configHelper;
    StatusNotifier *notifier;

    QLocalServer *instanceServer;
    bool instanceRunning;
    void initSingleInstance();

    void newProfile(Connection *);
    void editRow(int row);
    void blockChildrenSignals(bool);
    void checkCurrentIndex();
    void setupActionIcon();

    static const QUrl issueUrl;

private slots:
    void onImportGuiJson();
    void onExportGuiJson();
    void onSaveManually();
    void onAddManually();
    void onAddScreenQRCode();
    void onAddScreenQRCodeCapturer();
    void onAddQRCodeFile();
    void onAddFromURI();
    void onAddFromConfigJSON();
    void onDelete();
    void onEdit();
    void onShare();
    void onConnect();
    void onForceConnect();
    void onDisconnect();
    void onConnectionStatusChanged(const int row, const bool running);
    void onLatencyTest();
    void onMoveUp();
    void onMoveDown();
    void onGeneralSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void onAbout();
    void onReportBug();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onFilterToggled(bool);
    void onFilterTextChanged(const QString &text);
    void onQRCodeCapturerResultFound(const QString &uri);
    void onSingleInstanceConnect();

protected slots:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
