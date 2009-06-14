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

void BringWindowToTop(int winId);

class KServer : public QObject
{
    Q_OBJECT

public:

    class Widget
    {
    public:
        QString id;
        QString name;
        QString path;
        int pid;
        bool active;
        bool enabled;
    };

    enum WidgetIPC {
        ShowWindow = 0x8000,
        HideWindow = 0x8001,
        ShowHUD = 0x8002,
        HideHUD = 0x8003,
        SettingsChanged
    };

    KServer();

    static KServer* instance();

    bool initialize();
    void shutdown();

    QList<Widget>* widgets() { updateWidgetList(); return &widgetList; }
    void updateWidgetPID(const QString id, int pid);

    void showHUD();
    void hideHUD();

private:

    void loadDefaultWidgets();
    void setupMenu();
    void sendMessageToAll(long msg);
    void sendProcessMessage(int pid, long msg);
    void closeProcess(int pid);
    bool checkProcess(int pid);
    void updateSystemSettings();

    void updateWidgetList();
    void updateWidgetListPID();

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
    QList<QWidget*> hudScreens;
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

    QTimer updateTimer;
};

#endif // KLUDGETSERVER_H
