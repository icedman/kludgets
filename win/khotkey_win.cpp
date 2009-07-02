#include "khotkey.h"
#include "klog.h"
#include "config_win.h"

int win32Modifier(Qt::KeyboardModifier modifier)
{
    switch (modifier)
    {
    case Qt::ShiftModifier:
        return MOD_SHIFT;
    case Qt::AltModifier:
        return MOD_ALT;
    case Qt::ControlModifier:
        return MOD_CONTROL;
    }
    return 0;
}

int win32Key(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Tab:
        return VK_TAB;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_CapsLock:
        return VK_CAPITAL;
    case Qt::Key_NumLock:
        return VK_NUMLOCK;
    case Qt::Key_ScrollLock:
        return VK_SCROLL;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;
    }
    return key;
}

void HotKey::registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id)
{
    if (!RegisterHotKey(
                winId(),
                id,
                win32Modifier(modifier),
                win32Key(key)))
    {
        KLog::log("HotKey::unable to register hotkey");
    }
}

void HotKey::unregisterHotKey(int id)
{
    if (registeredKeys.contains(id))
    {
        registeredKeys.remove(id);
    }
    UnregisterHotKey(winId(), id);
}

bool HotKey::winEvent(MSG *message, long *result)
{
    switch (message->message)
    {
    case WM_HOTKEY:
        emit hotKeyPressed((Qt::Key)0, (Qt::KeyboardModifier)0);
        return true;
    }
    return false;
}
