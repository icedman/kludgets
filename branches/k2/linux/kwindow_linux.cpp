#include "kwindow.h"
#include "kview.h"
#include "klog.h"
#include "kserver.h"
#include "kipc.h"

void KWindow::updateWindowLevel(int l)
{
    if (l == 1)
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    else if (l == 2)
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnBottomHint);
    else
        setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen);

    if (l == 2)
        lower();
    else
        raise();
}

void KWindow::updateMouseIgnore(bool ignore)
{
}

void KWindow::moveToTop()
{
    raise();
}

void KWindow::moveToBottom()
{
    lower();
}
