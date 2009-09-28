#include "config_win.h"
#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"
#include "kipc.h"

#include <windows.h>

void raiseWindow(HWND hWnd, bool raise = true)
{
    QWidget topMost;
    if (!raise)
    {
        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
    }
    else
    {
        SetWindowPos(topMost.winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        //SetWindowPos(topMost.winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(hWnd, topMost.winId(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
    }
}

void KWindow::moveToTop()
{
    raiseWindow(winId());
}

void KWindow::moveToBottom()
{
    lower();
    raiseWindow(winId(), false);
}

void KWindow::updateWindowLevel(int l)
{
    if (l == 2)
    {
        lower();
    }
    else
    {
        raise();
    }

    if (l == 1)
        SetWindowPos(winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    else
        SetWindowPos(winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
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
            if (!isVisible())
                emit onShow();
            show();
            moveToTop();
            updateMouseIgnore(false);
            return true;
        }
    case KIPC::HideHUD:
        {
            moveToBottom();
            updateWindowLevel(windowZ);
            updateMouseIgnore(noMouse);
            return true;
        }
    case KIPC::ShowWindow:
        {
            emit onShow();
            show();
			moveToTop();
            updateWindowLevel(windowZ);
            return true;
        }
    case KIPC::HideWindow:
        {
            emit onHide();
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
