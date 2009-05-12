#include "config.h"
#include "version.h"
#include "kserver.h"
#include "kclient.h"
#include "ksettings.h"
#include "prefwindow.h"
#include "kdocument.h"
#include "util.h"
#include "klog.h"

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
        processLock(new QSharedMemory("kludget::server", this)),
        prefWindow(0),
        settings(new KSettings(this))
{
    settings->setRootKey("kludget");
}

bool KServer::initialize()
{
    if (!processLock->create(1))
        return false;

    QString enginePreferencesFile(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);
    if (!QFile::exists(enginePreferencesFile))
    {
        loadDefaultWidgets();
    }

    updateWidgetList();

    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        if ((*it).active)
        {
            widgetQueue.push_back((*it).path);
        }
        it++;
    }

    setupMenu();

    connect(this, SIGNAL(selectWidget(int)), this, SLOT(showWidget(int)));
    connect(&runningWidgetsMapper, SIGNAL(mapped(int)), this, SIGNAL(selectWidget(int)));
    connect(this, SIGNAL(selectInstalledWidget(QString)), this, SLOT(runWidget(QString)));
    connect(&installedWidgetsMapper, SIGNAL(mapped(QString)), this, SIGNAL(selectInstalledWidget(QString)));
    connect(&trayIcon, SIGNAL(messageClicked()), this, SLOT(openPackage()));

    settings->setPath(enginePreferencesFile);
    settings->loadPreferences(":resources/xml/enginePreferences.xml");

    KLog::instance()->clear();
    KLog::instance()->write("KServer::initialize");

    processWidgetQueue();
    return true;
}

void KServer::shutdown()
{
    QDir directory;
    QStringList dirs;
    QStringList::iterator dit;

    QList<Widget>::iterator it;

    // definitely remove these directories of uninstalled widgets
    directory.setPath(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/widgets/installed/");
    dirs = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    dit = dirs.begin();
    while (dit != dirs.end())
    {
        QString filePath = directory.absolutePath() + "/" + *dit;
        dit++;

        bool found = false;
        it = widgetList.begin();
        while (it != widgetList.end())
        {
            if ((*it).path.indexOf(filePath) == 0)
            {
                found = true;
                break;
            }
            it++;
        }

        if (!found)
        {
            Util::deleteDir(filePath);
        }
    }

    // close all widgets
    it = widgetList.begin();
    while (it != widgetList.end())
    {
        int pid = (*it).pid;
        if (pid > 0)
            closeProcess(pid);
        it++;
    }

    KLog::instance()->write("KServer::shutdown");
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
        if (fileInfo.suffix() == "zip")
        {
            KClient::installPackage(filePath);
        }
        fit++;
    }

    // run widget manager
}

void KServer::setupMenu()
{
    connect(&trayMenu, SIGNAL(aboutToShow()), this, SLOT(showMenu()));

    trayIcon.setIcon(QIcon(":resources/images/kludget.png"));
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();
}

void KServer::processWidgetQueue()
{
    if (widgetQueue.size())
    {
        QStringList::iterator it = widgetQueue.begin();
        runWidget((*it));
        widgetQueue.erase(it);
        QTimer::singleShot(800, this, SLOT(processWidgetQueue()));
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
        widget.name = doc.getValue("kludget/name", "");
        widget.path = doc.getValue("kludget/path", "");
        widget.pid = doc.getValue("kludget/pid", "").toInt();
        widget.active = false;

        if (!QFile::exists(widget.path))
            continue;

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

        if (!checkProcess(widget.pid))
            widget.pid = 0;

        widgetList.push_back(widget);
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
    QString path = QFileDialog::getOpenFileName(0,
                   "Open widget package",
                   QApplication::applicationDirPath() + "/widgets",
                   "Zipped Package(*.zip);;Kludget Package(*.kludget)");
    if (path != "")
        runWidget(path);
}

void KServer::showMenu()
{
    updateWidgetList();

    trayMenu.clear();
    connect(trayMenu.addAction(QString(KLUDGET_APPLICATION) + " v" + KLUDGET_MAJOR_VERSION), SIGNAL(triggered()), this, SLOT(goToEngineSite()));
    connect(trayMenu.addAction(QString("Preferences")), SIGNAL(triggered()), this, SLOT(configure()));
    trayMenu.insertSeparator(0);

    connect(trayMenu.addAction(QString("Open widget package...")), SIGNAL(triggered()), this, SLOT(openPackage()));

    widgetsMenu.clear();
    widgetsMenu.setTitle("Add widget");

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

        if (checkProcess(pid))
        {
            QAction *action = trayMenu.addAction(name);
            connect(action, SIGNAL(triggered()), &runningWidgetsMapper, SLOT(map()));
            runningWidgetsMapper.setMapping(action, pid);
            hasRunningWidgets = true;
        }

        it++;
    }

    if (hasRunningWidgets > 0)
    {
        trayMenu.insertSeparator(0);
        connect(trayMenu.addAction(QString("Show all widgets")), SIGNAL(triggered()), this, SLOT(showAllWidgets()));
        trayMenu.insertSeparator(0);
    }

    connect(trayMenu.addAction(QString("Exit")), SIGNAL(triggered()), this, SLOT(exit()));
}

void KServer::showWidget(int pid)
{
    sendProcessMessage(pid, ShowWindow);
}

void KServer::showAllWidgets()
{
    sendMessageToAll(ShowWindow);
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
    prefWindow->buildPreferenceMap(":resources/xml/enginePreferences.xml");
    prefWindow->setupUI();
    prefWindow->show();

    connect(prefWindow, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(prefWindow, SIGNAL(destroyed()), this, SLOT(onPreferencesClosed()));
}

void KServer::goToWidgetsSite()
{
    QDesktopServices::openUrl(QUrl("http://www.kludgets.com/widgets"));
}

void KServer::goToEngineSite()
{
    QDesktopServices::openUrl(QUrl("http://www.kludgets.com"));
}

void KServer::onSettingsChanged()
{
    updateSystemSettings();
    sendMessageToAll(SettingsChanged);
}

void KServer::onPreferencesClosed()
{
    prefWindow = 0;
}

void KServer::exit()
{
    shutdown();
    QTimer::singleShot(2000, QApplication::instance(), SLOT(quit()));
}
