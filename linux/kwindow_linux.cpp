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
    else
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen);

    if (l == 2)
        lower();
    else
        raise();
}

void KWindow::updateMouseIgnore(bool ignore)
{
}

#if 0
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
            case KIPC::HideHUD:
                {
                    qDebug("HideHUD");
                    lower();
                    return true;
                }
            case KIPC::ShowHUD:
            case KIPC::ShowWindow:
                {
                    qDebug("ShowWindow");
                    hide();
                    show();
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

#endif
