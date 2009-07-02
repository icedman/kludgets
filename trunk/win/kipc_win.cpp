#include "kipc.h"
#include "config_win.h"

typedef struct _sendMsgParam
{
    int pid;
    long msg;
}
sendMsgParam;

BOOL IsKludgetHWND(HWND hWnd)
{
    // very crude & inaccurate
    return (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED);
}

BOOL CALLBACK EnumWindowProc_SendMessage(HWND hWnd, LPARAM lParam)
{
    sendMsgParam *p = (sendMsgParam*)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    if (p->pid == 0 || pid == p->pid)
    {
        if (IsKludgetHWND(hWnd))
        {
            SendMessage(hWnd, p->msg, 0, 0);
        }
    }

    return TRUE;
}

BOOL CALLBACK EnumWindowProc_isKludget(HWND hWnd, LPARAM lParam)
{
    sendMsgParam *p = (sendMsgParam*)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    if (p->pid == 0 || pid == p->pid)
    {
        TCHAR className[MAX_PATH];
        GetClassName(hWnd, className, MAX_PATH);

        if (IsKludgetHWND(hWnd) || QString::fromWCharArray(className) == "QWidget")
        {
            p->msg = 1;
        }
    }

    return TRUE;
}

void KIPC::sendMessage(Message msg, int pId)
{
    sendMsgParam p;
    p.pid = pId;
    p.msg = msg;
    EnumWindows(EnumWindowProc_SendMessage, (LPARAM)&p);
}

void KIPC::sendAllMessage(Message msg)
{
    sendMsgParam p;
    p.pid = 0;
    p.msg = msg;
    EnumWindows(EnumWindowProc_SendMessage, (LPARAM)&p);
}

void KIPC::closeProcess(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                  PROCESS_ALL_ACCESS,
                                  FALSE, pid);
    if (hProcess == NULL)
        return ;
    if (!TerminateProcess(hProcess, 0))
        CloseHandle(hProcess);
}

bool KIPC::checkProcess(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                  PROCESS_VM_READ,
                                  FALSE, pid);
    if (hProcess == NULL)
        return false;

    sendMsgParam p;
    p.pid = pid;
    p.msg = 0;
    EnumWindows(EnumWindowProc_isKludget, (LPARAM)&p);

    CloseHandle(hProcess);

    return (p.msg == 1);
}
