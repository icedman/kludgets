#include "config.h"
#include "kserver.h"
#include "kclient.h"
#include "ksettings.h"
#include "knetwork.h"
#include "prefwindow.h"
#include "installwindow.h"
#include "kdocument.h"
#include "kutil.h"
#include "klog.h"
#include "khotkey.h"
#include "khudwindow.h"
#include "kapp.h"
#include "version.h"

#include <QDirIterator>
#include <QApplication>
#include <QDesktopServices>
#include <QSharedMemory>

#define KLUDGET_ENGINE_APP_ID "com.kludgets.kengine"

KServer* KServer::instance()
{
    static KServer inst;
    return &inst;
}

KServer::KServer() :
        aboutWindow(0),
        prefWindow(0),
        hotKeyListener(0),
        updateTimer(this),
        settings(new KSettings(this))
{
    settings->setRootKey("kludget");
}

bool KServer::initialize()
{
    if (KIPC::checkProcess(KIPC::getProcessId(KLUDGET_ENGINE_APP_ID)))
        return false;

    KIPC::setProcessId(KLUDGET_ENGINE_APP_ID, (int)QApplication::applicationPid());

    QString enginePreferencesFile(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);
    if (!QFile::exists(enginePreferencesFile))
    {
        loadDefaultWidgets();
    }

    updateWidgetList();

    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if ((*it).active && (*it).enabled)
        {
            widgetQueue.push_back((*it).path);
        }
        it++;
    }

    connect(this, SIGNAL(selectWidget(int)), this, SLOT(showWidget(int)));
    connect(&runningWidgetsMapper, SIGNAL(mapped(int)), this, SIGNAL(selectWidget(int)));
    connect(this, SIGNAL(selectInstalledWidget(QString)), this, SLOT(runWidget(QString)));
    connect(&installedWidgetsMapper, SIGNAL(mapped(QString)), this, SIGNAL(selectInstalledWidget(QString)));

    connect(&trayIcon, SIGNAL(messageClicked()), this, SLOT(openPackage()));

    KNetwork *net = KNetwork::instance();
    connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(onCheckDone(QNetworkReply*)));

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(checkUpdate()));
    updateTimer.setInterval(1000 * 60 * 60 * 24);

    hotKeyListener = new HotKey(this);
    connect(hotKeyListener, SIGNAL(hotKeyPressed(Qt::Key, Qt::KeyboardModifier)), this, SLOT(hotKeyPressed(Qt::Key, Qt::KeyboardModifier)));

    settings->setPath(enginePreferencesFile);
#if defined(WIN32)

    settings->loadPreferences(":resources/xml/enginePreferences.xml");
#else

    settings->loadPreferences(":resources/xml/enginePreferences_linux.xml");
#endif

    KLog::instance()->loadSettings();
    KLog::instance()->clear();
    KLog::log("KServer::initialize");

    updateSystemSettings();

    processWidgetQueue();

    setupMenu();
    return true;
}

void KServer::shutdown()
{
    KIPC::destroyPIDFile(KLUDGET_ENGINE_APP_ID);

    hideHUD();

    trayIcon.hide();

    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        int pid = (*it).pid;
        if (pid > 0)
            KIPC::closeProcess(pid);

        KIPC::destroyPIDFile((*it).id);
        it++;
    }

    hotKeyListener->deleteLater();

    KLog::log("KServer::shutdown");
}

void KServer::sendMessageToAll(KIPC::Message msg)
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        int pid = (*it).pid;
        if (pid > 0)
            ipc.sendMessage(msg, pid);
        it++;
    }
}

void KServer::loadDefaultWidgets()
{
    // todo: have this as an absolute list.. for security

    QDir directory(QApplication::applicationDirPath() + "/widgets/");
    QStringList files = directory.entryList(QDir::Files);
    QStringList::iterator fit = files.begin();

    while (fit != files.end())
    {
        QString filePath = directory.absolutePath() + "/" + *fit;
        // qDebug("%s", qPrintable(filePath));
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix() == "zip")
        {
            KClient::installPackage(filePath);
        }
        fit++;
    }

    // todo: run widget manager
}

void KServer::setupMenu()
{
    connect(&trayMenu, SIGNAL(aboutToShow()), this, SLOT(showMenu()));

    trayIcon.setIcon(QIcon(":resources/images/windows.png"));
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();
}

void KServer::checkUpdate()
{
    if (settings->read("general/checkForUpdates", 0).toInt() == 0)
        return ;

    QString lastCheckString = settings->read("general/lastCheck", "").toString();
    if (lastCheckString != "")
    {
        QDate lastCheck = QDate::fromString(lastCheckString, Qt::TextDate);
        lastCheck = lastCheck.addDays(1);
        if (lastCheck >= QDate::currentDate())
        {
            return ;
        }
    }

    KNetwork *net = KNetwork::instance();
    net->loadSettings();
    net->setAccess(true, false, QUrl());

    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://kludgets.com/checkUpdate.php")));
    request.setRawHeader("User-Agent", KApp::userAgent().toUtf8());

    net->get
    (request);
}

void KServer::hotKeyPressed(Qt::Key, Qt::KeyboardModifier)
{
    updateWidgetList();

#if 1 // defined(WIN32)

    if (hudScreens.size() > 0)
    {
        hideHUD();
    }
    else
    {
        showHUD();
    }
#else
    showAllWidgets();
#endif
}

void KServer::processWidgetQueue()
{
    if (widgetQueue.size())
    {
        QStringList::iterator it = widgetQueue.begin();
        runWidget((*it));
        widgetQueue.erase(it);
        QTimer::singleShot(200, this, SLOT(processWidgetQueue()));
    }
    else
    {
        checkUpdate();
    }
}

void KServer::updateWidgetList()
{
    widgetList.clear();

    QDir directory(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/widgets/");
    QStringList dirs = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    QStringList::iterator it = dirs.begin();
    while (it != dirs.end())
    {
        QString filePath = directory.absolutePath() + "/" + *it;
        it++;

        QString configFile(filePath + QString("/") + CONFIG_FILE);
        if (!QFile::exists(configFile))
            continue;

        KDocument doc;
        if (!doc.openDocument(configFile))
            continue;

        Widget widget;
        widget.id = doc.getValue("kludget/id", "");
        widget.pid = 0;
        widget.name = doc.getValue("kludget/name", "");
        widget.path = doc.getValue("kludget/path", "");
        widget.enabled = (doc.getValue("kludget/enabled", "1").toInt() != 0);
        widget.active = false;

        if (!QFile::exists(widget.path))
            continue;

        if (!QFile::exists(widget.path + "/kludget.xml"))
            continue;

        // has a running widget;
        if (widget.enabled)
        {
            QDirIterator instDir(filePath);
            while (instDir.hasNext())
            {
                instDir.next();
                QString instanceFile(instDir.filePath() + QString("/") + PREFERENCE_FILE);
                if (QFile::exists(instanceFile))
                {
                    widget.active = true;
                    break;
                }
            }
        }

        widgetList.push_back(widget);
    }

    updateWidgetListPID();
}

void KServer::updateWidgetListPID()
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        (*it).pid = ipc.getProcessId((*it).id);
        it++;
    }
}

void KServer::runWidget(const QString &path)
{
    QStringList args;
    args.push_back(path);
    QProcess *process = new QProcess(this);
    process->start(QApplication::applicationFilePath(), args, QIODevice::ReadOnly);
}

void KServer::openPackage()
{
    static QString lastPath = QApplication::applicationDirPath() + "/widgets";

    QString path = QFileDialog::getOpenFileName(0,
                   "Open widget package",
                   lastPath,
                   "Zipped Package(*.zip);;Kludget Package(*.kludget)");

    if (QFile::exists(path))
        lastPath = QDir(path).absolutePath();

    if (path != "")
        runWidget(path);
}

void KServer::showMenu()
{
    updateWidgetList();

    trayMenu.clear();
    connect(trayMenu.addAction(QString(KLUDGET_APPLICATION) + " v" + KApp::version()), SIGNAL(triggered()), this, SLOT(aboutKludget()));
    connect(trayMenu.addAction(QString("Preferences")), SIGNAL(triggered()), this, SLOT(configure()));
    trayMenu.insertSeparator(0);

    connect(trayMenu.addAction(QString("Open widget package...")), SIGNAL(triggered()), this, SLOT(openPackage()));

    widgetsMenu.clear();
    widgetsMenu.setTitle("Add widget");

    connect(trayMenu.addAction(QString("Find more widgets...")), SIGNAL(triggered()), this, SLOT(goToWidgetsSite()));

    bool hasNonRunningWidgets = false;

    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if (!(*it).active || (*it).pid == 0)
        {
            QAction *action = widgetsMenu.addAction((*it).name);
            connect(action, SIGNAL(triggered()), &installedWidgetsMapper, SLOT(map()));
            installedWidgetsMapper.setMapping(action, (*it).path);
            hasNonRunningWidgets = true;
        }
        it++;
    }

    if (hasNonRunningWidgets)
    {
        widgetsMenu.insertSeparator(0);
        connect(widgetsMenu.addAction(QString("Clear this list")), SIGNAL(triggered()), this, SLOT(uninstallWidgets()));
        trayMenu.addMenu(&widgetsMenu);
    }

    trayMenu.insertSeparator(0);
    bool hasRunningWidgets = false;

    it = widgetList.begin();
    while (it != widgetList.end())
    {
        QString name = (*it).name;
        int pid = (*it).pid;

        if (KIPC::checkProcess(pid))
        {
            QAction *action = trayMenu.addAction(name);
            connect(action, SIGNAL(triggered()), &runningWidgetsMapper, SLOT(map()));
            runningWidgetsMapper.setMapping(action, pid);
            hasRunningWidgets = true;
        }

        it++;
    }

    if (hasRunningWidgets)
    {
        trayMenu.insertSeparator(0);
        connect(trayMenu.addAction(QString("Show all widgets")), SIGNAL(triggered()), this, SLOT(showAllWidgets()));
        trayMenu.insertSeparator(0);
    }

    connect(trayMenu.addAction(QString("Exit")), SIGNAL(triggered()), this, SLOT(exit()));
}

void KServer::showWidget(int pid)
{
    ipc.sendMessage(KIPC::ShowWindow, pid);
}

void KServer::showAllWidgets()
{
    sendMessageToAll(KIPC::ShowWindow);
}

void KServer::uninstallWidgets()
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if (!(*it).active)
        {
            QString path = (*it).path;
            Util::deleteDir(path);
        }
        it++;
    }
}

void KServer::showHUD()
{
    QDesktopWidget desktop;

    sendMessageToAll(KIPC::HideWindow);

    for (int i = 0; i < desktop.numScreens(); i++)
    {
        QRect rect = desktop.screenGeometry(i);
        HudWindow *w = new HudWindow;
        w->resize(rect.width(), rect.height());
        w->move(rect.x(), rect.y());
        w->show();
        hudScreens.push_back(w);
    }

    sendMessageToAll(KIPC::ShowHUD);
    //QTimer::singleShot(800, this, SLOT(showAllWidgets()));
}

void KServer::hideHUD()
{
    QList<QWidget*>::iterator it = hudScreens.begin();
    while (it != hudScreens.end())
    {
        QWidget *w = *it;
        w->hide();
        w->deleteLater();
        it++;
    }
    hudScreens.clear();

    sendMessageToAll(KIPC::HideHUD);
}

void KServer::configure()
{
    if (prefWindow)
    {
        prefWindow->show();
        prefWindow->raise();
        return ;
    }

    prefWindow = new PreferenceWindow(settings);
    prefWindow->setAttribute(Qt::WA_DeleteOnClose);
    prefWindow->setWindowTitle("Preferences");
#if defined(WIN32)

    prefWindow->buildPreferenceMap(":resources/xml/enginePreferences.xml");
#else

    prefWindow->buildPreferenceMap(":resources/xml/enginePreferences_linux.xml");
#endif

    prefWindow->setupUI();
    prefWindow->show();

    connect(prefWindow, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(prefWindow, SIGNAL(destroyed()), this, SLOT(onPreferencesClosed()));
}

void KServer::aboutKludget()
{
    if (aboutWindow)
    {
        aboutWindow->show();
        aboutWindow->raise();
        return ;
    }

    aboutWindow = new AboutKludgetWindow();
    aboutWindow->setAttribute(Qt::WA_DeleteOnClose);
    aboutWindow->setWindowTitle("Kludget Engine");
    aboutWindow->setupUI();
    aboutWindow->show();

    connect(aboutWindow, SIGNAL(destroyed()), this, SLOT(onAboutClosed()));
}

void KServer::goToWidgetsSite()
{
    QDesktopServices::openUrl(QUrl("http://www.kludgets.com/widgets"));
}

void KServer::onSettingsChanged()
{
    KLog::instance()->loadSettings();
    updateSystemSettings();
    sendMessageToAll(KIPC::SettingsChanged);
}

void KServer::onAboutClosed()
{
    aboutWindow = 0;
}

void KServer::onPreferencesClosed()
{
    prefWindow = 0;
}

void KServer::onCheckDone(QNetworkReply *reply)
{
    int result = QString(reply->readAll()).toInt();
    if (result == 1)
    {
        QMessageBox msg;
        msg.setText("A new version of Kludget Engine is available.");
        msg.setInformativeText("Do you want to update now?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msg.exec();

        QDate lastCheck = QDate::currentDate();
        settings->write("general/lastCheck", QDate::currentDate().toString(Qt::TextDate));

        switch (ret)
        {
        case QMessageBox::Yes:
            QDesktopServices::openUrl(QUrl("http://www.kludgets.com/download"));
            break;
        case QMessageBox::No:
            break;
        }

        return ;
    }
}

void KServer::exit()
{
    shutdown();
    QTimer::singleShot(800, QApplication::instance(), SLOT(quit()));
}
