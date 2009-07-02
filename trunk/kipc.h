#ifndef KIPC_H
#define KIPC_H

#include <QString>

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
        SettingsChanged
    };

    void sendMessage(Message msg, int pId);
    void sendAllMessage(Message msg);

    static int getProcessId(const QString &kludgetId);
    static bool setProcessId(const QString &kludgetId, int pid);
    static void closeProcess(int pid);
    static bool checkProcess(int pid);
    static QString getPIDFile(const QString &kludgetId);
    static void destroyPIDFile(const QString &kludgetId);
};

#endif // KIPC_H
