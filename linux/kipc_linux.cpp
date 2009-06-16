#include "kipc.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QProcess>

#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

static long getSimpleProperty(Window w, Atom a)
{
    Atom real_type;
    int format;
    unsigned long n, extra, res = 0;
    int status;
    unsigned char *p = 0;

    status = XGetWindowProperty(QX11Info::display(), w, a, 0L, 1L, False, XA_CARDINAL,
                                &real_type, &format, &n, &extra, &p);

    if (p && (format == 32))
    {
        res = *(unsigned long*)p;
        XFree(p);
    }
    return res;
}

void sendIPCMessage(KIPC::Message msg, int w)
{
    static Atom a = 0;
    if (a == 0)
        a = XInternAtom(QX11Info::display(), "_KLUDGET_IPC_ATOM", True);
    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.display = QX11Info::display();
    ev.xclient.window = (Window) w;
    ev.xclient.message_type = a;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = 0;
    ev.xclient.data.l[1] = msg;
    XSendEvent(QX11Info::display(), (Window) w, False, 0L, &ev);
}

void KIPC::sendMessage(Message msg, int pId)
{
    unsigned int i, nrootwins;
    Window dw1, dw2, *rootwins = 0;
    Display *dpy = QX11Info::display();
    int screen_count = ScreenCount(dpy);

    Atom a = XInternAtom(QX11Info::display(), "_NET_WM_PID", False);

    for (int s = 0; s < screen_count; s++) {
        Window root = RootWindow(dpy, s);
        XQueryTree(dpy, root, &dw1, &dw2, &rootwins, &nrootwins);  
        for (i = 0; i < nrootwins; i++)
        {
            if (getSimpleProperty(rootwins[i], a) ==  pId || pId == 0)
            {
                sendIPCMessage(msg, rootwins[i]);
            }
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
    if (checkProcess(pid))
    {
        QProcess process;
        QString cmd = QString("kill ") + QString::number(pid);
        process.start(cmd, QIODevice::ReadOnly);
        process.waitForStarted(1000);
        process.waitForFinished(3000);
    }
}

bool KIPC::checkProcess(int pid)
{
    QProcess process;
    QString cmd = QString("ps ") + QString::number(pid);
    process.start(cmd, QIODevice::ReadOnly);
    process.waitForStarted(1000);
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();

    if (output.contains(QString::number(pid)) && output.contains("Kludgets"))
    {
        return true;
    }

    return false;
}
