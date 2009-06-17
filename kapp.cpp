#include "kapp.h"
#include "kclient.h"
#include "kserver.h"
#include "hotkey.h"
#include "kdocument.h"
#include "config.h"

#include <QDesktopServices>

KApp::KApp(int argc, char *argv[]) :
        QApplication(argc, argv)
{
    setOrganizationName("MindHyve");
    setApplicationName("Kludgets");
    setQuitOnLastWindowClosed(false);

    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).mkdir("widgets");
    QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QString("/widgets")).mkdir("installed");
}

bool KApp::startClient(const QString &path)
{
    KClient *client = KClient::instance();
    return client->initialize(path);
}

bool KApp::startServer()
{
    KServer *server = KServer::instance();
    return server->initialize();
}

bool KApp::startHotKeyListener()
{
    HotKey hotKeyListener;
    
    QString enginePreferencesFile(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);
    KDocument *doc = new KDocument;
    if (doc->openDocument(enginePreferencesFile))
    {
        hotKeyListener.registerHotKey(doc->getValue("general/hotKey", ""));
    }
    
    delete doc;
    hotKeyListener.run();
    return true;
}
