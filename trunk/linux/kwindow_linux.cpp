#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"
#include "kipc.h"

#include <QApplication>
#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void KWindow::updateWindowLevel(int l)
{
    if (l == 1)
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    else if (l == 2)
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnBottomHint);
}

void KWindow::updateMouseIgnore(bool ignore)
{
}

bool KWindow::x11Event(XEvent * ev)
{
    static Atom a = 0;
    if (a == 0)
        a = XInternAtom(QX11Info::display(), "_KLUDGET_IPC_ATOM", True);

    if (ev->xclient.type == ClientMessage)
    {
        if (ev->xclient.message_type == a)
        {
            switch(ev->xclient.data.l[1])
            {
            case KIPC::ShowHUD:
                {
                    qDebug("ShowHUD");
                    updateWindowLevel(1);
                    updateMouseIgnore(false);
                    return true;
                }
            case KIPC::HideHUD:
                {
                    qDebug("HideHUD");
                    lower();
                    updateWindowLevel(windowZ);
                    updateMouseIgnore(noMouse);
                    return true;
                }
            case KIPC::ShowWindow:
                {
                    qDebug("ShowWindow");
                    //SetForegroundWindow(winId());
                    hide();
                    show();
                    XRaiseWindow(QX11Info::display(), winId());
                    if (windowZ == 2)
                        lower();
                    else
                        raise();
                    return true;
                }
            case KIPC::HideWindow:
                {
                    qDebug("HideWindow");
                    hide();
                    return true;
                }
            case KIPC::LowerWindow:
                {
                    qDebug("LowerWindow");
                    lower();
                    return true;
                }
            case KIPC::SettingsChanged:
                {
                    qDebug("SettingsChanged");
                    emit onSettingsChanged();
                    return true;
                }
            }
        }
    }
    return false;
}
