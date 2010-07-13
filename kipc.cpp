#include "kipc.h"
#include "kapp.h"
#include "klog.h"
#include "kdocument.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>

#include <QLocalSocket>

#define KLUDGET_IPC_NAME "kludgetsipc"
#define MAX_IPC_INDEX 100

KIPCServer::KIPCServer(QObject *parent) : QObject(parent)
{
    _server = new QLocalServer(this);
    listen();
}

KIPCServer::~KIPCServer()
{}

void KIPCServer::listen()
{
    for(int i = 0; i<MAX_IPC_INDEX; i++) {
        QString ipcName = QString("%1%2").arg(KLUDGET_IPC_NAME).arg(i);
        if (_server->listen(ipcName)) {
            connect(_server, SIGNAL(newConnection()),
                    this, SLOT(onProcessNewConnection()));
            KLog::log(QString("ipc server listening %1").arg(ipcName));
            break;
        }
    }
}

void KIPCServer::onProcessNewConnection()
{
    while (_server->hasPendingConnections()) {
        KIPCClient *client = new KIPCClient(this, _server->nextPendingConnection());
        client->sendMessage("ping");
    }
}

bool KIPCServer::sendMessage(QString message, QString id, QString instance)
{
    QList<KIPCClient*> clients = findChildren<KIPCClient*>();
    bool hasSent = false;
    for (int i = 0; i<clients.length(); i++) {
        KLog::log(QString("sending %1").arg(clients[i]->isConnected()));
        hasSent = clients[i]->sendMessage(message, id, instance) || hasSent;
    }

    for (int i = 0; i<clients.length(); i++) {
        if (!clients[i]->isConnected()) {
            delete clients[i];
            clients = findChildren<KIPCClient*>();
            i = 0;
        }
    }
    return hasSent;
}

KIPCClient::KIPCClient(QObject *parent, QLocalSocket *socket) : QObject(parent), _socket(socket), _ipcIndex(0)
{
    if (!_socket) {
        _socket = new QLocalSocket(this);
        connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateConnection()));
    }
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

KIPCClient::~KIPCClient()
{}

bool KIPCClient::isConnected()
{
    return (_socket->state() == QLocalSocket::ConnectedState);
}

void KIPCClient::onUpdateConnection()
{
    if (_socket->state() == QLocalSocket::UnconnectedState) {
        QString ipcName = QString("%1%2").arg(KLUDGET_IPC_NAME).arg(_ipcIndex);
        _socket->connectToServer(ipcName);
        if (_socket->waitForConnected(500))
            KLog::log("ipc client connected");
        else {
            KLog::log("ipc client unable to connect");
            _ipcIndex++;
            if (_ipcIndex > MAX_IPC_INDEX)
                _ipcIndex = 0;
        }   
    }
}

void KIPCClient::onReadyRead()
{
    KDocument doc;
    doc.setContent(QString(_socket->readAll()));

    QString id = doc.getValue("msg/id", "");
    QString instance = doc.getValue("msg/instance", "");
    QString msg = doc.getValue("msg", "");

    KLog::log(QString("ipc read: %1").arg(msg));

    if (msg == "pong") {
        _id = id;
        _instance = instance;
        KLog::log(QString("pong id: %1").arg(id));
        KLog::log(QString("pong instance: %1").arg(instance));
    }

    emit messageReceived(msg, id, instance);
}

bool KIPCClient::sendMessage(QString message, QString id, QString instance)
{
    KLog::log("sendMessage");

    if (id.length() && _id.length()) {
        if (id != _id) {
            return false;
        }
    }
    
    QString msg = QString("<msg id=\"%1\" instance=\"%2\">%3</msg>").
                  arg(id).
                  arg(instance).
                  arg(message);

    KLog::log(QString("ipc write: %1").arg(msg));

    bool sent = _socket->write(msg.toUtf8());
    _socket->flush();
    return sent;
}

void KIPCClient::connectToServer()
{   
    _updateTimer.start(2000);
    onUpdateConnection();
}

int KIPC::getProcessId(const QString &kludgetId)
{
    int pid = 0;
    QString pidfile = getPIDFile(kludgetId);
    if (QFile::exists(pidfile))
    {
        QFile file;
        file.setFileName(pidfile);
        if (file.open(QIODevice::ReadOnly))
        {
            QString text = file.readAll();
            file.close();
            pid = text.toInt();
        }
    }

    // qDebug(">>%s %d", qPrintable(kludgetId), pid);

    if (!checkProcess(pid))
        return 0;

    return pid;
}

bool KIPC::setProcessId(const QString &kludgetId, int pid)
{
    QString pidfile = getPIDFile(kludgetId);
    QFile file;
    file.setFileName(pidfile);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QString::number(pid).toUtf8());
        file.write("\r\n", 2);
        file.close();
        return true;
    }
    return false;
}

QString KIPC::getPIDFile(const QString &kludgetId)
{
    return KApp::temporaryDirPath() + "/" + kludgetId + ".kludget.pid";
}

void KIPC::destroyPIDFile(const QString &kludgetId)
{
    QString pidfile = getPIDFile(kludgetId);
    if (QFile::exists(pidfile))
        QFile::remove
        (pidfile);

}
