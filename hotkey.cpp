#include "hotkey.h"

HotKey::HotKey(QObject *parent)
{}

QString HotKey::keyName(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Backspace:
        return ""; // "Backspace";
    case Qt::Key_Tab:
        return "Tab";
    case Qt::Key_Enter:
    case Qt::Key_Return:
        return "Enter";
    case Qt::Key_Escape:
        return ""; //"Escape";
    case Qt::Key_Space:
        return "Space";
    case Qt::Key_Exclam:
        return 0x21;
    case Qt::Key_QuoteDbl:
        return 0x22;
    case Qt::Key_NumberSign:
        return 0x23;
    case Qt::Key_Dollar:
        return 0x24;
    case Qt::Key_Percent:
        return 0x25;
    case Qt::Key_Ampersand:
        return 0x26;
    case Qt::Key_Apostrophe:
        return 0x27;
    case Qt::Key_ParenLeft:
        return 0x28;
    case Qt::Key_ParenRight:
        return 0x29;
    case Qt::Key_Asterisk:
        return 0x2a;
    case Qt::Key_Plus:
        return 0x2b;
    case Qt::Key_Comma:
        return 0x2c;
    case Qt::Key_Minus:
        return 0x2d;
    case Qt::Key_Period:
        return 0x2e;
    case Qt::Key_Slash:
        return 0x2f;
    case Qt::Key_0:
        return 0x30;
    case Qt::Key_1:
        return 0x31;
    case Qt::Key_2:
        return 0x32;
    case Qt::Key_3:
        return 0x33;
    case Qt::Key_4:
        return 0x34;
    case Qt::Key_5:
        return 0x35;
    case Qt::Key_6:
        return 0x36;
    case Qt::Key_7:
        return 0x37;
    case Qt::Key_8:
        return 0x38;
    case Qt::Key_9:
        return 0x39;
    case Qt::Key_Colon:
        return 0x3a;
    case Qt::Key_Semicolon:
        return 0x3b;
    case Qt::Key_Less:
        return 0x3c;
    case Qt::Key_Equal:
        return 0x3d;
    case Qt::Key_Greater:
        return 0x3e;
    case Qt::Key_Question:
        return 0x3f;
    case Qt::Key_At:
        return 0x40;
    case Qt::Key_A:
        return 0x41; // 0x41 == 'A', 0x61 == 'a'
    case Qt::Key_B:
        return 0x42;
    case Qt::Key_C:
        return 0x43;
    case Qt::Key_D:
        return 0x44;
    case Qt::Key_E:
        return 0x45;
    case Qt::Key_F:
        return 0x46;
    case Qt::Key_G:
        return 0x47;
    case Qt::Key_H:
        return 0x48;
    case Qt::Key_I:
        return 0x49;
    case Qt::Key_J:
        return 0x4a;
    case Qt::Key_K:
        return 0x4b;
    case Qt::Key_L:
        return 0x4c;
    case Qt::Key_M:
        return 0x4d;
    case Qt::Key_N:
        return 0x4e;
    case Qt::Key_O:
        return 0x4f;
    case Qt::Key_P:
        return 0x50;
    case Qt::Key_Q:
        return 0x51;
    case Qt::Key_R:
        return 0x52;
    case Qt::Key_S:
        return 0x53;
    case Qt::Key_T:
        return 0x54;
    case Qt::Key_U:
        return 0x55;
    case Qt::Key_V:
        return 0x56;
    case Qt::Key_W:
        return 0x57;
    case Qt::Key_X:
        return 0x58;
    case Qt::Key_Y:
        return 0x59;
    case Qt::Key_Z:
        return 0x5a;
    case Qt::Key_BracketLeft:
        return 0x5b;
    case Qt::Key_Backslash:
        return 0x5c;
    case Qt::Key_BracketRight:
        return 0x5d;
    case Qt::Key_AsciiCircum:
        return 0x5e;
    case Qt::Key_Underscore:
        return 0x5f;
    case Qt::Key_QuoteLeft:
        return 0x60;

    case Qt::Key_BraceLeft:
        return 0x7b;
    case Qt::Key_Bar:
        return 0x7c;
    case Qt::Key_BraceRight:
        return 0x7d;
    case Qt::Key_AsciiTilde:
        return 0x7e;

    case Qt::Key_Delete:
        return ""; // "Delete";
    case Qt::Key_Insert:
        return "Insert"; // = 0x1006,

    case Qt::Key_Home:
        return "Home"; // = 0x1010,        // cursor movement
    case Qt::Key_End:
        return "End"; // = 0x1011,
    case Qt::Key_Left:
        return "Left"; // = 0x1012,
    case Qt::Key_Up:
        return "Up"; // = 0x1013,
    case Qt::Key_Right:
        return "Right"; // = 0x1014,
    case Qt::Key_Down:
        return "Down"; // = 0x1015,
    case Qt::Key_PageUp:
        return "Page Up"; // = 0x1016,
    case Qt::Key_PageDown:
        return "Page Down"; // = 0x1017,
    case Qt::Key_CapsLock:
        return "Caps Lock"; // = 0x1024,
    case Qt::Key_NumLock:
        return "Num Lock"; // = 0x1025,
    case Qt::Key_ScrollLock:
        return "Scroll Lock"; // = 0x1026,

    case Qt::Key_F1:
        return "F1"; // = 0x1030,        // function keys
    case Qt::Key_F2:
        return "F2"; // = 0x1031,
    case Qt::Key_F3:
        return "F3"; // = 0x1032,
    case Qt::Key_F4:
        return "F4"; // = 0x1033,
    case Qt::Key_F5:
        return "F5"; // = 0x1034,
    case Qt::Key_F6:
        return "F6"; // = 0x1035,
    case Qt::Key_F7:
        return "F7"; // = 0x1036,
    case Qt::Key_F8:
        return "F8"; // = 0x1037,
    case Qt::Key_F9:
        return "F9"; // = 0x1038,
    case Qt::Key_F10:
        return "F10"; // = 0x1039,
    case Qt::Key_F11:
        return "F11"; // = 0x103a,
    case Qt::Key_F12:
        return "F12"; // = 0x103b,
    case Qt::Key_F13:
        return "F13"; // = 0x103c,
    case Qt::Key_F14:
        return "F14"; // = 0x103d,
    case Qt::Key_F15:
        return "F15"; // = 0x103e,
    case Qt::Key_F16:
        return "F16"; // = 0x103f,
    case Qt::Key_F17:
        return "F17"; // = 0x1040,
    case Qt::Key_F18:
        return "F18"; // = 0x1041,
    case Qt::Key_F19:
        return "F19"; // = 0x1042,
    case Qt::Key_F20:
        return "F20"; // = 0x1043,
    case Qt::Key_F21:
        return "F21"; // = 0x1044,
    case Qt::Key_F22:
        return "F22"; // = 0x1045,
    case Qt::Key_F23:
        return "F23"; // = 0x1046,
    case Qt::Key_F24:
        return "F24"; // = 0x1047,
    case Qt::Key_F25:
        return "F25"; // = 0x1048,        // F25 .. F35 only on X11
    case Qt::Key_F26:
        return "F26"; // = 0x1049,
    case Qt::Key_F27:
        return "F27"; // = 0x104a,
    case Qt::Key_F28:
        return "F28"; // = 0x104b,
    case Qt::Key_F29:
        return "F29"; // = 0x104c,
    case Qt::Key_F30:
        return "F30"; // = 0x104d,
    case Qt::Key_F31:
        return "F31"; // = 0x104e,
    case Qt::Key_F32:
        return "F32"; // = 0x104f,
    case Qt::Key_F33:
        return "F33"; // = 0x1050,
    case Qt::Key_F34:
        return "F34"; // = 0x1051,
    case Qt::Key_F35:
        return "F35"; // = 0x1052,
    }
    return "";
}
