#ifndef KIPC_H
#define KIPC_H

#include <QString>

class KIPC
{
public:

    enum Message {
        ShowWindow = 0x8000,
        HideWindow,
        LowerWindow,
        ShowHUD,
        HideHUD,
        SettingsChanged
    };

    void sendMessage(Message msg, int pId);
    void sendAllMessage(Message msg);
    int getProcessId(QString kludgetId);

    static void closeProcess(int pid);
    static bool checkProcess(int pid);
};

#endif // KIPC_H
