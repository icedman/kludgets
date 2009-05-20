#ifndef KLUDGETSERVER_H
#define KLUDGETSERVER_H

#include <QProcess>
#include <QStringList>
#include <QtGui>
#include <QSignalMapper>

#include "kludgetinfo.h"

class KSettings;
class PreferenceWindow;
class AboutKludgetWindow;
class HotKey;
class QNetworkReply;

class KServer : public QObject
{
    Q_OBJECT

public:

    enum WidgetIPC {
        ShowWindow = 0x8000,
        HideWindow = 0x8001,
        SettingsChanged
    };

    KServer();

    static KServer* instance();

    bool initialize();
    void shutdown();

private:

    class Widget
    {
    public:
        QString id;
        QString name;
        QString path;
        int pid;
        bool active;
    };

    void loadDefaultWidgets();
    void setupMenu();
    void updateWidgetList();
    void sendMessageToAll(long msg);
    void sendProcessMessage(int pid, long msg);
    void closeProcess(int pid);
    bool checkProcess(int pid);
    void updateSystemSettings();

    private
Q_SLOTS:
    void checkUpdate();
    void hotKeyPressed(Qt::Key, Qt::KeyboardModifier);
    void processWidgetQueue();
    void runWidget(const QString &path);
    void openPackage();
    void showMenu();
    void showWidget(int);
    void showAllWidgets();
    void uninstallWidgets();
    void configure();
    void aboutKludget();
    void goToWidgetsSite();
    void onSettingsChanged();
    void onAboutClosed();
    void onPreferencesClosed();
    void onCheckDone(QNetworkReply *reply);
    void exit();

Q_SIGNALS:
    void selectWidget(int);
    void selectInstalledWidget(QString);

private:
    QList<Widget> widgetList;
    QStringList widgetQueue;

    QMenu trayMenu;
    QMenu widgetsMenu;
    QSystemTrayIcon trayIcon;
    QSignalMapper runningWidgetsMapper;
    QSignalMapper installedWidgetsMapper;

    PreferenceWindow *prefWindow;
    AboutKludgetWindow *aboutWindow;
    KSettings *settings;

    QSharedMemory *processLock;

    HotKey *hotKeyListener;
    bool hotKeyShow;

    QTimer updateTimer;
};

#endif // KLUDGETSERVER_H
