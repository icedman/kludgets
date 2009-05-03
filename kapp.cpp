#include "kapp.h"
#include "kclient.h"
#include "kserver.h"

KApp::KApp(int argc, char *argv[]) :
        QApplication(argc, argv)
{
    setOrganizationName("MindHyve");
    setApplicationName("Kludgets");
    setQuitOnLastWindowClosed(false);
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
