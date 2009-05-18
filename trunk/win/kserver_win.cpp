#include "kserver.h"
#include "ksettings.h"
#include "config_win.h"
#include "windowsregistry.h"
#include "hotkey.h"

#include <QApplication>

typedef struct _sendMsgParam
{
    int pid;
    long msg;
}
sendMsgParam;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    sendMsgParam *p = (sendMsgParam*)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == p->pid)
    {
        if (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED)
        {
            switch (p->msg)
            {
            case KServer::ShowWindow:
                {
                    SetForegroundWindow(hWnd);
                    SendMessage(hWnd, p->msg, 0, 0);
                    //BringWindowToTop(hWnd);
                    //ShowWindow(hWnd, SW_SHOW);
                    break;
                }
            default:
                SendMessage(hWnd, p->msg, 0, 0);
            }
        }
    }
    return TRUE;
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
    EnumWindows(EnumWindowsProc, (LPARAM)&p);
}

void KServer::closeProcess(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                  PROCESS_ALL_ACCESS,
                                  FALSE, pid);
    if (hProcess == NULL)
        return ;
    if (!TerminateProcess(hProcess, 0))
        CloseHandle(hProcess);
}

bool KServer::checkProcess(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                  PROCESS_VM_READ,
                                  FALSE, pid);
    if (hProcess == NULL)
        return false;
    CloseHandle(hProcess);
    return true;
}

void KServer::updateSystemSettings()
{
    // run at start up
    WindowsRegistry reg;
    reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 1);
    if (settings->read("general/runAtStartup", 0).toInt() != 0)
    {
        TCHAR szRunPath[MAX_PATH];
        int len = QApplication::applicationFilePath().toWCharArray(szRunPath);
        szRunPath[len] = 0;
        reg.WriteString(_T("KludgetEngine"), szRunPath);
    }
    else
    {
        reg.DeleteValue(_T("KludgetEngine"));
    }
    reg.Close();

    // hotkey
    QString hotkey = settings->read("general/hotKey", "").toString() + ":0";
    int hotKey = hotkey.split(":")[0].toInt();
    int hotKeyModifier = hotkey.split(":")[1].toInt();
    hotKeyListener->registerHotKey((Qt::Key)hotKey, (Qt::KeyboardModifier)hotKeyModifier);
}
