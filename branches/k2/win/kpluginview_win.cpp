#include "kpluginview.h"
#include "kwindow.h"
#include "kview.h"

#include <QEvent>

#include <windows.h>

bool KPluginView::findAndHostPlugins(KWindow *window, KView *webview)
{
    HWND hwnd = FindWindowEx(webview->winId(), NULL, L"WebPluginView", NULL);
    if (hwnd)
    {
        KPluginView *v = new KPluginView(window, webview);
        v->windowId = (int)hwnd;
        SetParent(hwnd, v->winId());
        return true;
    }
    return false;
}

void KPluginView::closeEvent(QCloseEvent *ev)
{
    if (!view)
        return ;

    HWND hwnd = (HWND)windowId;
    if (IsWindow(hwnd))
        SetParent(hwnd, view->winId());
}

void KPluginView::onUpdate()
{
    HWND hwnd = (HWND)windowId;
    if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE) || view->isTransitioning())
    {
        close();
        return ;
    }

    if (size() != view->size())
    {
        if (lastUpdate.elapsed() < 2000)
            return ;

        lastUpdate.restart();
        resize(view->width(), view->height());

        RECT rect;
        GetWindowRect(hwnd, &rect);
        SetWindowRgn(
            winId(),
            CreateRectRgn(
                rect.left - x(),
                rect.top - y(),
                rect.right - x(),
                rect.bottom - y()),
            true);
        return ;
    }

    SetWindowPos(parent->winId(), winId(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);

    if (parent->x() != x() || parent->y() != y())
    {
        move(parent->x(), parent->y());
        return ;
    }

    bool parentVisible = parent->isVisible();
    bool thisVisible = isVisible();

    if (parentVisible && !thisVisible)
        show();
    else if (!parentVisible && thisVisible)
        hide();
}
