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
            widgetQueue.push_back((*it).package);
        }
        it++;
    }

    connect(this, SIGNAL(selectWidget(int)), this, SLOT(showWidget(int)));
    connect(&runningWidgetsMapper, SIGNAL(mapped(int)), this, SIGNAL(selectWidget(int)));
    connect(this, SIGNAL(selectInstalledWidget(QString)), this, SLOT(runWidget(QString)));
    connect(&installedWidgetsMapper, SIGNAL(mapped(QString)), this, SIGNAL(selectInstalledWidget(QString)));
	connect(this, SIGNAL(selectConfigureWidget(QString)), this, SLOT(configureWidget(QString)));
	connect(&configureWidgetMapper, SIGNAL(mapped(QString)), this, SIGNAL(selectConfigureWidget(QString)));

    // connect(&trayIcon, SIGNAL(messageClicked()), this, SLOT(openPackage()));
    connect(&trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

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

    Util::deleteDir(KApp::temporaryDirPath());
    KLog::log("KServer::shutdown");
}

void KServer::sendMessageToAll(KIPC::Message msg)
{
    updateWidgetList();
    ipcServer.sendMessage(QString("%1").arg(msg));
}

void KServer::loadDefaultWidgets()
{
    QDir directory(QApplication::applicationDirPath() + "/widgets/");
    QStringList files = directory.entryList(QDir::Files);
    QStringList::iterator fit = files.begin();

    while (fit != files.end())
    {
        QString filePath = directory.absolutePath() + "/" + *fit;
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix() == "zip" || fileInfo.suffix() == "wgt")
            KClient::registerPackage(filePath);
        fit++;
    }

    runWidget(directory.absolutePath() + "/com.kludgets.clock.zip");
    runWidget(directory.absolutePath() + "/com.kludgets.calendar.zip");
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
#if defined(WIN32)

    if (hudScreens.size() > 0)
        hideHUD();
    else
        showHUD();

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
        widget.package = doc.getValue("kludget/package", "");
        widget.storage = doc.getValue("kludget/storage", "");
        widget.enabled = (doc.getValue("kludget/enabled", "1").toInt() != 0);
        widget.active = false;

        if (!QFile::exists(widget.package))
            continue;

        // has a running widget;
        if (widget.enabled)
        {
            QDirIterator instDir(widget.storage);
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
    KIPC ipc;
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        (*it).pid = ipc.getProcessId((*it).id);
        it++;
    }
}

void KServer::runWidget(const QString &path)
{
#if 1
		QStringList args;
		args.push_back(path);
		QProcess *process = new QProcess(this);
		process->start(QApplication::applicationFilePath(), args, QIODevice::ReadOnly);
#else
		// run everyone on the same process
		// todo
		KClient *k = new KClient;
		k->initialize(path);
#endif
}

void KServer::configureWidget(const QString &widgetId)
{
	QStringList widget = widgetId.split(":");
	QString id = widget[0];
	QString instance = "";
	if (widget.size() > 1)
		instance = widget[1];
	
	ipcServer.sendMessage(QString("%1").arg(KIPC::ShowOptions), id, instance);
}

void KServer::openPackage()
{
    static QString lastPath = QApplication::applicationDirPath() + "/widgets";

    QString path = QFileDialog::getOpenFileName(0,
                   "Open widget package",
                   lastPath,
                   "Zipped Package(*.zip);;Kludget Package(*.kludget)");//;;Widget Package(*.wgt)");

    if (QFile::exists(path))
        lastPath = QDir(path).absolutePath();

    if (path != "")
        runWidget(path);
}

void KServer::trayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r == QSystemTrayIcon::DoubleClick)
    {
        KLog::log("KServer::trayActivated");
#if defined(WIN32)

        showHUD();
#else

        showAllWidgets();
#endif

    }
}

void KServer::showMenu()
{
    updateWidgetList();

    trayMenu.clear();
    connect(trayMenu.addAction(QString(KLUDGET_APPLICATION) + " v" + KApp::version()), SIGNAL(triggered()), this, SLOT(aboutKludget()));
    connect(trayMenu.addAction(QString("Preferences")), SIGNAL(triggered()), this, SLOT(configure()));
    trayMenu.insertSeparator(0);

    connect(trayMenu.addAction(QString("Open widget package...")), SIGNAL(triggered()), this, SLOT(openPackage()));

    addWidgetsMenu.clear();
    addWidgetsMenu.setTitle("Add widget");

	widgetsMenu.clear();
    widgetsMenu.setTitle("Widget Options");

    connect(trayMenu.addAction(QString("Find more widgets...")), SIGNAL(triggered()), this, SLOT(goToWidgetsSite()));

    bool hasNonRunningWidgets = false;

    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if (!(*it).active || (*it).pid == 0)
        {
            QAction *action = addWidgetsMenu.addAction((*it).name);
            connect(action, SIGNAL(triggered()), &installedWidgetsMapper, SLOT(map()));
            installedWidgetsMapper.setMapping(action, (*it).package);
            hasNonRunningWidgets = true;
        }
        it++;
    }

    if (hasNonRunningWidgets)
    {
        addWidgetsMenu.insertSeparator(0);
        connect(addWidgetsMenu.addAction(QString("Clear this list")), SIGNAL(triggered()), this, SLOT(clearRecentPackages()));
        trayMenu.addMenu(&addWidgetsMenu);
    }

    trayMenu.insertSeparator(0);
    bool hasRunningWidgets = false;

    it = widgetList.begin();
    while (it != widgetList.end())
    {
		Widget &widget = *it;
        QString name = widget.name;
        int pid = widget.pid;
        it++;

		QStringList instances = ipcServer.getInstances(widget.id);

        if (KIPC::checkProcess(pid) && instances.size() > 0)
        {
            QAction *action = trayMenu.addAction(name);
            connect(action, SIGNAL(triggered()), &runningWidgetsMapper, SLOT(map()));
            runningWidgetsMapper.setMapping(action, pid);
            hasRunningWidgets = true;

			for(int i = 0; i<instances.size(); i++) {
				QString name = widget.name;
				QString widgetId = widget.id;
				if (instances.size() > 1) {
					name += ":";
					name += instances.at(i);
					widgetId += ":";
					widgetId += instances.at(i);
				}
				QAction *action = widgetsMenu.addAction(name);
				connect(action, SIGNAL(triggered()), &configureWidgetMapper, SLOT(map()));
				configureWidgetMapper.setMapping(action, widgetId);
			}

        }
    }

    if (hasRunningWidgets)
    {
        trayMenu.insertSeparator(0);
        connect(trayMenu.addAction(QString("Show all widgets")), SIGNAL(triggered()), this, SLOT(showAllWidgets()));
		if (hasRunningWidgets)
			trayMenu.addMenu(&widgetsMenu);
        trayMenu.insertSeparator(0);
    }

    connect(trayMenu.addAction(QString("Exit")), SIGNAL(triggered()), this, SLOT(exit()));
}

void KServer::showWidget(int pid)
{
    //ipc.sendMessage(KIPC::ShowWindow, pid);
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if (pid == (*it).pid)
            ipcServer.sendMessage(QString("%1").arg(KIPC::ShowWindow), QString("%1").arg((*it).id));
        it++;
    }
}

void KServer::showAllWidgets()
{
    sendMessageToAll(KIPC::ShowWindow);
}

void KServer::clearRecentPackages()
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if (!(*it).active)
        {
            QString path = (*it).storage;
            Util::deleteDir(path);
        }
        it++;
    }
}

void KServer::showHUD()
{
    updateWidgetList();

    QDesktopWidget desktop;

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
    QTimer::singleShot(1000, QApplication::instance(), SLOT(quit()));
}
