#include "kserver.h"
#include "ksettings.h"
#include "hotkey.h"

#include <QApplication>

typedef struct _sendMsgParam
{
    int pid;
    long msg;
}
sendMsgParam;


void KServer::updateWidgetListPID()
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        (*it).pid = 0;
        it++;
    }
}

void KServer::sendMessageToAll(long msg)
{
    QList<Widget>::iterator it = widgetList.begin();
    while (it != widgetList.end())
    {
        int pid = (*it).pid;
        if (pid > 0)
            sendProcessMessage(pid, msg);
        it++;
    }
}

void KServer::sendProcessMessage(int pid, long msg)
{
    sendMsgParam p;
    p.pid = pid;
    p.msg = msg;
}

void KServer::closeProcess(int pid)
{
}

bool KServer::checkProcess(int pid)
{
    return false;
}

void KServer::updateSystemSettings()
{
}

