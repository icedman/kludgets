#include "kserver.h"
#include "ksettings.h"
#include "hotkey.h"

void KServer::updateSystemSettings()
{
    hotKeyListener->registerHotKey(settings->read("general/hotKey", "").toString());
}

