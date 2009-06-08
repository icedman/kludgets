#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"

#include <windows.h>

void KWindow::updateWindowLevel(int l)
{
    // todo. use windowFlags
    if (l == 1)
        SetWindowPos(winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    else
        SetWindowPos(winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    if (l == 2)
        lower();
    else
        raise();
}

void KWindow::updateMouseIgnore(bool ignore)
{
    DWORD exStyle = GetWindowLong(winId(), GWL_EXSTYLE);
    if (!ignore)
        exStyle = exStyle & ~WS_EX_TRANSPARENT;
    else
        exStyle = exStyle | WS_EX_TRANSPARENT;
    SetWindowLong(winId(), GWL_EXSTYLE, exStyle);
}

bool KWindow::winEvent(MSG *message, long *result)
{
    result = 0;
    switch (message->message)
    {
    case KServer::ShowHUD:
        {
            updateWindowLevel(1);
            updateMouseIgnore(false);
            return true;
        }
    case KServer::HideHUD:
        {
            updateWindowLevel(windowZ);
            updateMouseIgnore(noMouse);
            return true;
        }
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
