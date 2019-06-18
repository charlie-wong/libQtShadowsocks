#ifndef STATUSNOTIFIER_H
#define STATUSNOTIFIER_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

class MainWindow;

class StatusNotifier : public QObject {
    Q_OBJECT

public:
    StatusNotifier(MainWindow *w, bool startHiden, QObject *parent = 0);

public slots:
    void activate();
    void showNotification(const QString &);
    void onWindowVisibleChanged(bool visible);

private:
    QMenu systrayMenu;
    QAction *minimiseRestoreAction;
    QSystemTrayIcon systray;
    MainWindow *window;
};

#endif // STATUSNOTIFIER_H
