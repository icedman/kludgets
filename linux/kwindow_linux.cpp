#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"

void KWindow::updateWindowLevel(int l)
{
    // todo. use windowFlags
    if (l == 2)
        lower();
    else
        raise();
}

void KWindow::updateMouseIgnore(bool ignore)
{
}

