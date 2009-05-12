#include "config.h"
#include "kclient.h"
#include "knetwork.h"
#include "kludget.h"
#include "ksettings.h"
#include "kdocument.h"
#include "installwindow.h"
#include "util.h"
#include "klog.h"

#include <QWebSettings>
#include <QApplication>
#include <QFontDatabase>
#include <QDesktopServices>
#include <QProcess>
#include <QDir>

#define MAX_INSTANCES 100

KClient* KClient::instance()
{
    static KClient inst;
    return &inst;
}

bool KClient::initialize(const QString& path)
{
    // QApplication::setAttribute(Qt::AA_MSWindowsUseDirect3DByDefault, true);

    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).mkdir("widgets");
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QString("/widgets")).mkdir("installed");

    loadCustomFonts(QApplication::applicationDirPath() + "/widgets/Resources/Fonts");

    KNetwork::instance();

    QFileInfo fileInfo(path);
    QString appPath = fileInfo.absoluteFilePath();

    if (!QFile::exists(appPath))
        return false;

    if (!fileInfo.isDir())
    {
        QString suffix = fileInfo.suffix();
        if (suffix == "zip" || suffix == "kludget")
        {
            return installFromArchive(appPath);
        }
        return false;
    }
    else
    {
        // run directly from directory (todo. debug mode check)
        if (0)
        {
            if (appPath.indexOf(QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absolutePath() + "/widgets") != 0)
                return installFromDirectory(appPath);
        }
    }

    info = KludgetInfo(appPath, "");
    return run();
}

bool KClient::run()
{
    processLock = new QSharedMemory(QString("kludget::" + info.id), this);
    if (!processLock->create(1))
        return false;

    bool created = false;

    QDirIterator it(info.storagePath);
    while (it.hasNext())
    {
        it.next();

        QString instanceFile(it.filePath() + "/" + PREFERENCE_FILE);
        if (QFile::exists(instanceFile))
        {
            instanceQueue.push_back(it.fileName());
            created = true;
        }
    }

    if (!created)
        instanceQueue.push_back("0");

    // save pid file
    KDocument doc;
    doc.setValue("kludget/name", info.name);
    doc.setValue("kludget/id", info.id);
    doc.setValue("kludget/path", info.path);
    doc.setValue("kludget/pid", QString::number(QApplication::applicationPid()));
    doc.saveDocument(info.storagePath + "/" + CONFIG_FILE);

    // localization
    localefiles.initialize(info.path, "en");
    KNetwork::instance()->setLocaleFiles(&localefiles);

    loadCustomFonts(info.path + "/fonts");
    loadPlugins(info.path + "/plugins");

    QApplication::setQuitOnLastWindowClosed(false);

    return processInstanceQueue();
}

void KClient::shutdown()
{}

bool KClient::installPackage(const QString& path)
{
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).mkdir("widgets");
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QString("/widgets")).mkdir("installed");

    KClient client;
    client.installFromArchive(path, false);
    return true;
}

bool KClient::installFromArchive(const QString& path, bool prompt)
{
    QFileInfo fileInfo(path);
    QString suffix = fileInfo.suffix();

    QString out = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QString("/widgets/installed/") + fileInfo.fileName() + QString("/");
    out.replace("." + suffix, "");

    Util::extract(path, out);

    KludgetInfo tmp(out, "");

    if (tmp.configFile == "")
    {
        QDirIterator it(out);
        while (it.hasNext())
        {
            it.next();
            tmp = KludgetInfo(it.filePath(), "");
            if (tmp.configFile != "")
                break;
        }
    }

    return installWidget(tmp.path, prompt);
}

bool KClient::installFromDirectory(const QString& path, bool prompt)
{
    QString dest = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QString("/widgets/installed/") + QDir(path).dirName();
    if (Util::copyDir(path, dest, true))
    {
        return installWidget(QDir(dest).absolutePath(), prompt);
    }
    return false;
}

bool KClient::installWidget(const QString& path, bool prompt)
{
    info = KludgetInfo(path, "");

    // setup storage dir
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/widgets").mkpath(QDir(info.storagePath).dirName());

    KDocument pid;
    pid.setValue("kludget/name", info.name);
    pid.setValue("kludget/id", info.id);
    pid.setValue("kludget/path", info.path);
    pid.saveDocument(info.storagePath + "/" + CONFIG_FILE);

    KDocument config;
    if (!config.openDocument(info.configFile))
        return false;

    QString defaultAccess = config.getValue("widget/access/full", "false");
    bool local = config.getValue("widget/access/local", defaultAccess) == "true";
    bool network = config.getValue("widget/access/network", defaultAccess) == "true";
    bool plugins = config.getValue("widget/access/plugins", defaultAccess) == "true";
    bool system = config.getValue("widget/access/system", defaultAccess) == "true";

    KDocument access;
    if (local)
        access.setValue("kludget/access/local", "1");
    if (network)
        access.setValue("kludget/access/network", "1");
    if (plugins)
        access.setValue("kludget/access/plugins", "1");
    if (system)
        access.setValue("kludget/access/system", "1");
    access.saveDocument(info.storagePath + "/access.xml");

    if (!prompt)
        return true;

    InstallWindow *installWindow = new InstallWindow(info);
    installWindow->setAttribute(Qt::WA_DeleteOnClose);
    installWindow->setWindowTitle("Installation - " + info.name);

    if (local || network || plugins || system)
    {
        installWindow->buildPreferenceMap(":resources/xml/accessPreferences.xml", true);
    }

    installWindow->setupUI();
    installWindow->show();

    connect(installWindow, SIGNAL(saved()), this, SLOT(run()));
    connect(installWindow, SIGNAL(cancelled()), this, SLOT(exit()));

    QApplication::setQuitOnLastWindowClosed(true);
    return true;
}

void KClient::loadCustomFonts(const QString &path)
{
    QDirIterator it(path);
    while (it.hasNext())
    {
        it.next();
        QFontDatabase::addApplicationFont(it.filePath());
    }
}

void KClient::loadPlugins(const QString& path)
{}

Kludget* KClient::createInstance(const QString &instance)
{
    QString inst = instance;
    if (inst == "" && kludgets.length() > 0)
    {
        for (int i = 0; i < MAX_INSTANCES; i++)
        {
            inst = QString::number(i);
            if (!QFile(info.storagePath + "/" + inst + "/" + PREFERENCE_FILE).exists())
                break;
        }
    }

    Kludget *k = Kludget::create(this, KludgetInfo(info.path, inst));
    if (!k)
        return 0;

    kludgets.push_back(k);
    connect(k, SIGNAL(destroyed(QObject*)), this, SLOT(kludgetDestroyed(QObject*)));

    return k;
}

bool KClient::processInstanceQueue()
{
    if (instanceQueue.size())
    {
        QStringList::iterator it = instanceQueue.begin();
        createInstance((*it));
        instanceQueue.erase(it);
        QTimer::singleShot(800, this, SLOT(processInstanceQueue()));
        return true;
    }
    // no successfully running widgets
    if (kludgets.length() == 0)
        QApplication::instance()->quit();

    return false;
}

void KClient::kludgetDestroyed(QObject *k)
{
    kludgets.removeAll((Kludget*)k);
    if (kludgets.length() == 0)
    {
        exit();
    }
}

void KClient::exit()
{
    shutdown();
    QApplication::instance()->quit();
}
