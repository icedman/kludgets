#include "hotkey.h"
#include "klog.h"

void HotKey::registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id)
{
    KLog::log("HotKey::unable to register hotkey");
}

void HotKey::unregisterHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id)
{
}

