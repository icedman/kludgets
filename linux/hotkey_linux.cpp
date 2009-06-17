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

void HotKey::registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id)
{
    // not need to implement
}

void HotKey::unregisterHotKey(int id)
{
    if (registeredKeys.contains(id))
    {
        registeredKeys.remove(id);
    }
}

bool HotKey::run()
{
    Display *display = QX11Info::display();
    
    int keycode = XKeysymToKeycode(display, XStringToKeysym("F11"));
    // grab keys
    XGrabKey(display, keycode, ControlMask, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

    XEvent e;
    for(;;)
    {
        XNextEvent(display, &e);
        if (e.type == KeyPress){
            if (e.xkey.keycode == keycode)
                qDebug("heya!");
        }
    }
    
    // woudn't reach here, probably
    
    // ungrab keys
    XUngrabKey(display, keycode, 0, DefaultRootWindow(display));

    return true;
}
