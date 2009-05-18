#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"

#include <windows.h>

// todo. use windowFlags
void KWindow::setWindowLevel(int l)
{
    windowZ = l;
    if (l == 1)
        SetWindowPos(winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    else
        SetWindowPos(winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    if (l == 2)
        lower();
    else
        raise();
}

bool KWindow::winEvent(MSG *message, long *result)
{
    result = 0;
    switch (message->message)
    {
    case KServer::ShowWindow:
        {
            SetForegroundWindow(winId());
            show();
            if (windowZ == 2)
                lower();
            else
                raise();
            return true;
        }
    case KServer::HideWindow:
        {
            hide();
            return true;
        }
    case KServer::SettingsChanged:
        {
            emit onSettingsChanged();
            return true;
        }
    }
    return false;
}
