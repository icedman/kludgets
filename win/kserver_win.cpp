#include "kserver.h"
#include "ksettings.h"
#include "config_win.h"
#include "windowsregistry.h"
#include "hotkey.h"

#include <QApplication>

void KServer::updateSystemSettings()
{
    // run at start up
    WindowsRegistry reg;
    reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 1);
    if (settings->read("general/runAtStartup", 0).toInt() != 0)
    {
        TCHAR szRunPath[MAX_PATH];
        int len = QApplication::applicationFilePath().toWCharArray(szRunPath);
        szRunPath[len] = 0;
        reg.WriteString(_T("KludgetEngine"), szRunPath);
    }
    else
    {
        reg.DeleteValue(_T("KludgetEngine"));
    }
    reg.Close();

    // hotkey
    QString hotkey = settings->read("general/hotKey", "").toString() + ":0";
    int hotKey = hotkey.split(":")[0].toInt();
    int hotKeyModifier = hotkey.split(":")[1].toInt();
    hotKeyListener->registerHotKey((Qt::Key)hotKey, (Qt::KeyboardModifier)hotKeyModifier);
}

