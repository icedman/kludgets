#ifndef KLUDGETSERVER_H
#define KLUDGETSERVER_H

#include <QStringList>
#include <QtWidgets>
#include <QSignalMapper>

#include "kludgetinfo.h"
#include "kipc.h"

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
        QString package;
        QString storage;
        int pid;
        bool active;
        bool enabled;
    };

    KServer();

    static KServer* instance();

    bool initialize();
    void shutdown();

    QList<Widget>* widgets() {
        updateWidgetList();
        return &widgetList;
    }

    void showHUD();
    void hideHUD();

private:

    void sendMessageToAll(KIPC::Message msg);

    void loadDefaultWidgets();
    void setupMenu();
    void updateSystemSettings();
    void updateWidgetList();
    void updateWidgetListPID();

    private
Q_SLOTS:
    void checkUpdate();
    void hotKeyPressed(Qt::Key, Qt::KeyboardModifier);
    void processWidgetQueue();
    void runWidget(const QString &path);
	void configureWidget(const QString &id);
    void openPackage();
    void trayActivated(QSystemTrayIcon::ActivationReason r);
    void showMenu();
    void showWidget(int);
    void showAllWidgets();
    void clearRecentPackages();
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
	void selectConfigureWidget(QString);

private:
    QList<QWidget*> hudScreens;
    QList<Widget> widgetList;
    QStringList widgetQueue;

    QMenu trayMenu;
    QMenu widgetsMenu;
	QMenu addWidgetsMenu;
    QSystemTrayIcon trayIcon;
    QSignalMapper runningWidgetsMapper;
    QSignalMapper installedWidgetsMapper;
	QSignalMapper configureWidgetMapper;

    PreferenceWindow *prefWindow;
    AboutKludgetWindow *aboutWindow;
    KSettings *settings;

    HotKey *hotKeyListener;
    QTimer updateTimer;
    KIPCServer ipcServer;
};

#endif // KLUDGETSERVER_H
