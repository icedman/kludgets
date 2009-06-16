#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"

#include <QApplication>
#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void KWindow::updateWindowLevel(int l)
{
    // todo. use windowFlags
    if (l == 2)
        lower();
    else
        raise();

    // hack, put this somewhere
    Atom a = XInternAtom(QX11Info::display(), "_NET_WM_PID", True);
    unsigned long pid = QApplication::applicationPid();
    XChangeProperty(QX11Info::display(),
                    winId(),
                    a,
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char*)&pid,
                    1);

}

void KWindow::updateMouseIgnore(bool ignore)
{
}

bool KWindow::x11Event(XEvent * event)
{
    return false;
}
