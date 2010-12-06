#include "kserver.h"
#include "ksettings.h"
#include "config_win.h"
#include "windowsregistry.h"
#include "khotkey.h"

#include <QApplication>

void KServer::updateSystemSettings()
{
    // run at start up
    WindowsRegistry reg;
    reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 1);
    if (settings->read("general/runAtStartup", 0).toInt() != 0)
    {
        TCHAR szRunPath[MAX_PATH];
		QString runPath = QString("\"%1\"").arg(QApplication::applicationFilePath());
		runPath.replace("/","\\");
        int len = runPath.toWCharArray(szRunPath);
        szRunPath[len] = 0;
		
        reg.WriteString(_T("KludgetEngine"), szRunPath);
    }
    else
    {
        reg.DeleteValue(_T("KludgetEngine"));
    }
    reg.Close();

    // hotkey
    hotKeyListener->registerHotKey(settings->read("general/hotKey", "").toString());
}

