#include "kapp.h"
#include "kclient.h"
#include "kserver.h"
#include "khotkey.h"
#include "kdocument.h"
#include "config.h"
#include "klog.h"
#include "version.h"

#include <QDesktopServices>
#include <QDir>

KApp::KApp(int argc, char *argv[]) :
        QApplication(argc, argv)
{
    setOrganizationName("MindHyve");
    setApplicationName("Kludgets");
    setQuitOnLastWindowClosed(false);

	qDebug(qPrintable(QDesktopServices::storageLocation(QDesktopServices::DataLocation)));

    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).mkdir("widgets");
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).mkdir("temp");

	addLibraryPath(applicationDirPath() + "/plugins");
}

bool KApp::startClient(const QString &path)
{
    return KClient::instance()->initialize(path);
}

bool KApp::startServer()
{
    return KServer::instance()->initialize();
}

#if !defined(WIN32)
bool KApp::startHotKeyListener()
{
    HotKey hotKey;

    QString enginePreferencesFile(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);
    KDocument *doc = new KDocument;
    if (doc->openDocument(enginePreferencesFile))
    {
        hotKey.registerHotKey(doc->getValue("general/hotKey", ""));
    }

    delete doc;

    KLog::log("HotKeyListener::started");
    hotKey.run();

    return true;
}

#endif

QString KApp::userAgent()
{
    QString platform = "(Windows)";
#if !defined(WIN32)

    platform = "(Linux)";
#endif

    return QString(KLUDGET_MOZ_VERSION) + " "
           + platform + " "
           + KLUDGET_QT_VERSION + " "
           + KLUDGET_NAME + "/" + version();
}

QString KApp::version()
{
    return QString(KLUDGET_MAJOR_VERSION) + "." + KLUDGET_MINOR_VERSION;
}

QString KApp::temporaryDirPath()
{
    return QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absolutePath() + "/temp";
}
