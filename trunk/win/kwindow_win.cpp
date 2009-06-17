#include "config_win.h"
#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"
#include "kipc.h"

#include <windows.h>

void KWindow::updateWindowLevel(int l)
{
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
    case KIPC::ShowHUD:
        {
            updateWindowLevel(1);
            updateMouseIgnore(false);
            return true;
        }
    case KIPC::HideHUD:
        {
            lower();
            updateWindowLevel(windowZ);
            updateMouseIgnore(noMouse);
            return true;
        }
    case KIPC::ShowWindow:
        {
            show();
            SetForegroundWindow(winId());
            if (windowZ == 2)
                lower();
            else
                raise();
            return true;
        }
    case KIPC::HideWindow:
        {
            hide();
            return true;
        }
    case KIPC::LowerWindow:
        {
            lower();
            return true;
        }
    case KIPC::SettingsChanged:
        {
            emit onSettingsChanged();
            return true;
        }
    }
    return false;
}
