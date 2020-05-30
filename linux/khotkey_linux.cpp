#include "khotkey.h"
#include "klog.h"

#include <QProcess>
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
    #if 0
    Display *display = QX11Info::display();

    QList<int> keys;
    QList<int> keysQt;
    QList<int> modifiers;

    int i;

    for(i = 0; i<registeredKeys.values().size(); i++)
    {
        QString hotKeyString = registeredKeys.values().at(i) + ":0";

        int hotKey = hotKeyString.split(":")[0].toInt();
        int hotKeyModifier = x11Modifier((Qt::KeyboardModifier)hotKeyString.split(":")[1].toInt());
        QString hotKeyName = keyName((Qt::Key)hotKey);
        QByteArray ba = hotKeyName.toLatin1();
        keysQt.push_back(hotKey);
        hotKey = XKeysymToKeycode(display, XStringToKeysym(ba.data()));
        XGrabKey(display, hotKey, hotKeyModifier, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
        keys.push_back(hotKey);
        modifiers.push_back(hotKeyModifier);
    }

    XEvent e;
    for(;;)
    {
        XNextEvent(display, &e);
        if (e.type == KeyPress){

            for(i = 0; i<keys.size(); i++)
            {
                if (e.xkey.keycode == keys.at(i))
                {
                    // should output to stderr
                    qDebug("HotKey:%d:%d", e.xkey.keycode, 0);
                }
            }
        }
    }
    
    // wouldn't reach here, probably
    //XUngrabKey(display, keycode, 0, DefaultRootWindow(display));
    #endif
    return true;
}

bool HotKey::createExternalListener()
{
    destroyExternalListener();
    externalListener = new QProcess(this);

    connect(externalListener, SIGNAL(readyReadStandardOutput()), this, SLOT(externalListenerOutput()));
    connect(externalListener, SIGNAL(readyReadStandardError()), this, SLOT(externalListenerError()));

    externalListener->start("./Kludgets -l", QIODevice::ReadOnly);
    externalListener->waitForStarted(1000);
    return true;
}

void HotKey::destroyExternalListener()
{
    if (externalListener)
    {
        externalListener->terminate();
        externalListener->waitForFinished(1000);
        delete externalListener;
        externalListener = 0;
    }
}

void HotKey::externalListenerError()
{
    QString out = externalListener->readAllStandardError();
    QStringList key = out.split(":");
    if (key.size() == 3)
    {
        if (key[0] == "HotKey")
            emit hotKeyPressed((Qt::Key)0, (Qt::KeyboardModifier)0);
    }
    //qDebug("%s", qPrintable(out));
}

void HotKey::externalListenerOutput()
{
}
