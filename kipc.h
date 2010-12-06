#ifndef KIPC_H
#define KIPC_H

#include <QString>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>

class KIPCClient : public QObject
{
    Q_OBJECT
public:

    KIPCClient(QObject *parent = 0, QLocalSocket *socket = 0);
    ~KIPCClient();

    bool sendMessage(QString message, QString id = "", QString instance = "");
    bool isConnected();
    void connectToServer();

Q_SIGNALS:
    void messageReceived(QString message, QString id, QString instance);

private Q_SLOTS:
    void onUpdateConnection();
    void onReadyRead();

protected:
	friend class KIPCServer;
    QLocalSocket *_socket;
    QTimer _updateTimer;
    QString _id;
    QString _instance;
    int _ipcIndex;
};

class KIPCServer : public QObject
{
    Q_OBJECT
public:

    KIPCServer(QObject *parent = 0);
    ~KIPCServer();

    void listen();

    bool sendMessage(QString message, QString id = "", QString instance = "");
	QStringList getInstances(QString id);

private Q_SLOTS:
    void onProcessNewConnection();

private:
    QLocalServer *_server;
};


class KIPC
{
public:

    enum Message {
        EngineExit = 0x8001,
        ShowWindow,
        HideWindow,
        LowerWindow,
        ShowHUD,
        HideHUD,
		Configure,
		ShowOptions,
        SettingsChanged
    };

    static int getProcessId(const QString &kludgetId);
    static bool setProcessId(const QString &kludgetId, int pid);
    static void closeProcess(int pid);
    static bool checkProcess(int pid);
    static QString getPIDFile(const QString &kludgetId);
    static void destroyPIDFile(const QString &kludgetId);
};

#endif // KIPC_H
