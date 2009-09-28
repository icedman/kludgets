#include "config.h"
#include "kclient.h"
#include "knetwork.h"
#include "kludget.h"
#include "ksettings.h"
#include "kdocument.h"
#include "installwindow.h"
#include "kutil.h"
#include "klog.h"
#include "kipc.h"
#include "kapp.h"

#include <QWebSettings>
#include <QApplication>
#include <QFontDatabase>
#include <QDesktopServices>
#include <QDir>

#define MAX_INSTANCES 100

KClient* KClient::instance()
{
    static KClient inst;
    return &inst;
}

bool KClient::initialize(const QString& path)
{
    KLog::instance()->loadSettings();

    loadFonts(QApplication::applicationDirPath() + "/widgets/resources/fonts");

    KNetwork::instance();

    // run package
    QFileInfo fileInfo(path);
    QString appPath = fileInfo.absoluteFilePath();
    if (QFile::exists(appPath)) {
        if (!fileInfo.isDir()) {
            QString suffix = fileInfo.suffix();
            if (suffix == "zip" || suffix == "kludget") {
                if (loadPackage(appPath)) {
                    run();
                    return true;
                }
            }
            return false;
        }
    } else {
        KLog::log("KClient::initialize failure");
        KLog::log(QString("path not found: ") + appPath);
        return false;
    }

    // run directly from the directory
    info = KludgetInfo(appPath, "");
    info.load();
    return run();
}

void KClient::setEnabled(bool enabled)
{
    KDocument doc;
    doc.openDocument(info.storagePath + "/" + CONFIG_FILE);
	doc.setValue("kludget/enabled", enabled ? QString("1") : QString("0"));
    doc.saveDocument(info.storagePath + "/" + CONFIG_FILE);
}

bool KClient::isEnabled()
{
    KDocument doc;
    doc.openDocument(info.storagePath + "/" + CONFIG_FILE);
    return (doc.getValue("kludget/enabled", "1").toInt() != 0);
}

bool KClient::run()
{
    if (KIPC::checkProcess(KIPC::getProcessId(info.id)))
        return false;

    bool created = false;

    QDirIterator it(info.storagePath);
    while (it.hasNext()) {
        it.next();

        QString instanceFile(it.filePath() + "/" + PREFERENCE_FILE);
        if (QFile::exists(instanceFile)) {
            instanceQueue.push_back(it.fileName());
            created = true;
        }
    }

    if (!created)
        instanceQueue.push_back("0");

    // localization
    localefiles.initialize(info.path, "en");
    KNetwork::instance()->setLocaleFiles(&localefiles);

    // fonts and plugins
    loadFonts(info.path + "/fonts");
    loadPlugins(info.path + "/plugins");

    // enable
	setEnabled(true);

    KIPC::setProcessId(info.id, (int)QApplication::applicationPid());

    QApplication::setQuitOnLastWindowClosed(false);
    return processInstanceQueue();
}

void KClient::shutdown()
{
    KIPC::destroyPIDFile(info.id);
}

bool KClient::registerPackage(const QString& path)
{
    KClient client;
    return client.loadPackage(path);
}

bool KClient::loadPackage(const QString& path)
{
    QFileInfo fileInfo(path);
    QString suffix = fileInfo.suffix();

    QString out = KApp::temporaryDirPath() + "/" + fileInfo.fileName() + "/";
    out.replace("." + suffix, "");

    Util::extract(path, out);

    KludgetInfo tmp(out, "");
    tmp.load();

    if (tmp.configFile == "") {
        QDirIterator it(out);
        while (it.hasNext()) {
            it.next();
            tmp = KludgetInfo(it.filePath(), "");
            if (tmp.load())
                break;
        }
    }

    info = KludgetInfo(tmp.path, "");
    info.load();

    // setup storage dir
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/widgets").mkpath(QDir(info.storagePath).dirName());

    KDocument doc;
    doc.setValue("kludget/name", info.name);
    doc.setValue("kludget/id", info.id);
    doc.setValue("kludget/path", info.path);
    doc.setValue("kludget/package", path);
    doc.setValue("kludget/storage", QDir(info.storagePath).absolutePath());
    doc.setValue("kludget/enabled", "1");
    doc.saveDocument(info.storagePath + "/" + CONFIG_FILE);

    KDocument config;
    if (!config.openDocument(info.configFile)) {
        KLog::log("KClient::installWidget failure");
        KLog::log("config file not found");
        return false;
    }

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

#if 0
    InstallWindow *installWindow = new InstallWindow(info);
    installWindow->setAttribute(Qt::WA_DeleteOnClose);
    installWindow->setWindowTitle("Installation - " + info.name);

    if (local || network || plugins || system) {
        installWindow->buildPreferenceMap(":resources/xml/accessPreferences.xml", true);
    }

    installWindow->setupUI();
    installWindow->show();

    connect(installWindow, SIGNAL(saved()), this, SLOT(run()));
    connect(installWindow, SIGNAL(cancelled()), this, SLOT(exit()));
#endif

    return true;
}

void KClient::loadFonts(const QString &path)
{
    QDirIterator it(path);
    while (it.hasNext()) {
        it.next();
        QFontDatabase::addApplicationFont(it.filePath());
    }
}

void KClient::loadPlugins(const QString& path)
{}

Kludget* KClient::createInstance(const QString &instance)
{
    QString inst = instance;
    if (inst == "" && kludgets.length() > 0) {
        for (int i = 0; i < MAX_INSTANCES; i++) {
            inst = QString::number(i);
            if (!QFile(info.storagePath + "/" + inst + "/" + PREFERENCE_FILE).exists())
                break;
        }
    }

    if (getInstance(inst) != 0)
        return 0;

    KludgetInfo ki(info.path, inst);
    ki.load();
    Kludget *k = Kludget::create(this, ki);
    if (!k)
        return 0;

    kludgets.push_back(k);
    connect(k, SIGNAL(destroyed(QObject*)), this, SLOT(kludgetDestroyed(QObject*)));

    return k;
}

Kludget* KClient::getInstance(const QString &instance)
{
    KludgetList::iterator it = kludgets.begin();
    while (it != kludgets.end()) {
        Kludget *ik = (*it);
        if (ik->property("instance") == instance)
            return ik;
        it++;
    }
    return 0;
}

bool KClient::processInstanceQueue()
{
    if (instanceQueue.size()) {
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
    if (kludgets.length() == 0) {
		setEnabled(false);
        exit();
    }
}

void KClient::exit()
{
    shutdown();
    QApplication::instance()->quit();
}
