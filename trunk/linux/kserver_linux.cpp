#include "kserver.h"
#include "ksettings.h"
#include "khotkey.h"

void KServer::updateSystemSettings()
{
    hotKeyListener->createExternalListener();
}

