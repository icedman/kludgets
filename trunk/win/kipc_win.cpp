#include "kipc.h"
#include "config_win.h"

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
