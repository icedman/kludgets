#include "hudwindow.h"
#include "kserver.h"

HudWindow::HudWindow()
{
#if defined(WIN32)
    setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
#else
    setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen);
#endif
    setWindowOpacity(0.5);

    QPalette pal = palette();
    pal.setBrush(QPalette::Background, Qt::black);
    setPalette(pal);
}

void HudWindow::mousePressEvent(QMouseEvent * event)
{
    KServer::instance()->hideHUD();
}

void HudWindow::closeEvent(QCloseEvent * event)
{
    KServer::instance()->hideHUD();
}
