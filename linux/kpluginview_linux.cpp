#include "kpluginview.h"
#include "kwindow.h"
#include "kview.h"

#include <QEvent>


bool KPluginView::findAndHostPlugins(KWindow *window, KView *webview)
{
    return false;
}

void KPluginView::closeEvent(QCloseEvent *ev)
{
    if (!view)
        return ;
}

void KPluginView::onUpdate()
{
}
