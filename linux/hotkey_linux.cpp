#include "hotkey.h"
#include "klog.h"

#include <QThread>
#include <QX11Info>
#include <X11/Xlib.h>

unsigned int x11Modifier(Qt::KeyboardModifier modifier)
{
    switch (modifier)
    {
    case Qt::ShiftModifier:
        return ShiftMask;
    case Qt::AltModifier:
        return AnyModifier;
    case Qt::ControlModifier:
        return ControlMask;
    }
    return 0;
}

class HotKeyPrivate : public QThread
{
public:

    HotKeyPrivate() :
            QThread(0)
    {}

    void grab()
    {
        start();
    }

    void ungrab()
    {
        //XUngrabKey(dpy, F2, 0, root);
        quit();
    }

    void run()
    {
        Display *display = QX11Info::display();
        int keycode = XKeysymToKeycode(display, XStringToKeysym("Home"));
        XGrabKey(display, keycode, AnyModifier, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

        KLog::log("HotKey::registered hotkey");

        XEvent e;
        for(;;) {
            XNextEvent(display, &e);
            if (e.type == KeyPress){
                if (e.xkey.keycode == keycode)
                    qDebug("heya!");
            }
        }
    }
};

void HotKey::registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id)
{
    if (registeredKeys.contains(id))
    {
        unregisterHotKey(id);
    }
}

void HotKey::unregisterHotKey(int id)
{
    if (registeredKeys.contains(id))
    {
        registeredKeys.remove(id);
    }
}

bool HotKey::x11Event(XEvent * ev)
{
    return false;
}
