#include "kpluginview.h"
#include "kview.h"

KPluginView::KPluginView(KWindow *window, KView *webview) :
        parent(window),
        view(webview)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
    resize(1, 1);

    connect((QWidget*)parent, SIGNAL(destroyed()), this, SLOT(onParentDestroyed()));
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));

    updateTimer.start(50);
    lastUpdate.start();
}

void KPluginView::onParentDestroyed()
{
    parent = 0;
    view = 0;

    updateTimer.stop();
    close();
}
