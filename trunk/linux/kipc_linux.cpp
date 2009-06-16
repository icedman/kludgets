#include "kipc.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>

#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>

static long getSimpleProperty(Window w, Atom a)
{
    Atom real_type;
    int format;
    unsigned long n, extra, res = 0;
    int status;
    unsigned char *p = 0;

    status = XGetWindowProperty(QX11Info::display(), w, a, 0L, 1L, False, a,
                                &real_type, &format, &n, &extra, &p);
    if ((status == Success) && (n == 1) && (format == 32))
        res = *(unsigned long*)p;
    if (p) XFree(p);
    return res;
}

void sendIPCMessage(KIPC::Message msg, int w)
{

    static Atom a = 0;
    if (a == 0)
        a = XInternAtom(QX11Info::display(), "_KLUDGET_IPC_ATOM", False);
    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.display = QX11Info::display();
    ev.xclient.window = (Window) w;
    ev.xclient.message_type = a;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = msg;
    ev.xclient.data.l[1] = 0;
    XSendEvent(QX11Info::display(), (Window) w, False, 0L, &ev);
}

void KIPC::sendMessage(Message msg, int pId)
{
    unsigned int i, j, nrootwins, nchildwins;
    Window dw1, dw2, *rootwins = 0, *childwins = 0;
    Display *dpy = QX11Info::display();
    int screen_count = ScreenCount(dpy);

    Atom a = XInternAtom(QX11Info::display(), "_NET_WM_PID", False);

    for (int s = 0; s < screen_count; s++) {
        Window root = RootWindow(dpy, s);
        XQueryTree(dpy, root, &dw1, &dw2, &rootwins, &nrootwins);  
        for (i = 0; i < nrootwins; i++)
        {
            //qDebug("finding %d %d", pId, getSimpleProperty(rootwins[i], a));
            if (getSimpleProperty(rootwins[i], a) ==  pId || pId == 0)
            {
                sendIPCMessage(msg, rootwins[i]);
                qDebug("%d found", pId);
                continue;
            }

#if 0
            XQueryTree(dpy, rootwins[i], &dw1, &dw2, &childwins, &nchildwins);
            for (j = 0; j < nchildwins; j++)
            {
                if (getSimpleProperty(childwins[j], a) ==  pId)
                {
                    qDebug("finding child %d %d", pId, getSimpleProperty(childwins[j], a));
                }
            }
#endif
        }
        XFree((char *) rootwins);
    }
    XSync(dpy,False);
}

void KIPC::sendAllMessage(Message msg)
{
    sendMessage(msg, 0);
}

void KIPC::closeProcess(int pid)
{
}

bool KIPC::checkProcess(int pid)
{
    return true;
}
