#include "config.h"
#include "kwindow.h"
#include "kview.h"

KWindow::KWindow() :
        webView(new KView(this)),
        updateTimer(this),
        contentsSize(0, 0),
        dragging(false),
        noDrag(false),
        windowZ(0),
        alpha(255),
        snap(false),
        zoom(1),
        prevVisibility(false),
        autoSized(true)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupWindowProperties();
    setCentralWidget(webView);

    connect(webView, SIGNAL(bufferUpdated()), this, SLOT(update()));
    connect(webView->page(), SIGNAL(geometryChangeRequested(QRect)), this, SLOT(onGeometryChangeRequested(QRect)));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));

    updateTimer.start(50);
}

KView* KWindow::view()
{
    return webView;
}

void KWindow::closeEvent(QCloseEvent *ev)
{
    /*
       if (isVisible())
           ev->ignore();
    */
}

bool KWindow::isDragging()
{
    return dragging;
}

void KWindow::startDrag()
{
    dragPos = QCursor::pos() - pos();
    dragging = true;

    emit onStartDrag();
}

void KWindow::endDrag()
{
    dragging = false;

    emit onEndDrag();
}

bool KWindow::isIgnoreDrag()
{
    return noDrag;
}

void KWindow::setIgnoreDrag(bool drag)
{
    noDrag = drag;
}

void KWindow::onGeometryChangeRequested(const QRect & rect)
{
    setWindowState(windowState() & ~Qt::WindowMaximized);
    move(rect.x(), rect.y());
    resize(rect.width(), rect.height());
}

void KWindow::onUpdate()
{
    if (isDragging() && !isIgnoreDrag())
    {
        move(QCursor::pos() - dragPos);
        return ;
    }

    if (isVisible())
    {
        if (windowLevel() == 2)
            lower();

        if (isSnapToScreen())
            snapToScreen();

        if (isAutoSize())
            autoSize();
    }

    if (prevVisibility != isVisible())
    {
        if (isVisible())
            emit onShow();
        else
            emit onHide();
        prevVisibility = isVisible();
    }
}

void KWindow::setAutoSize(bool s)
{
    autoSized = s;
}

bool KWindow::isAutoSize()
{
    return autoSized;
}

void KWindow::autoSize(bool force)
{
    if (force)
        resize(32, 32);

    if (view()->zoomFactor() != zoomFactor())
    {
        view()->setZoomFactor(zoomFactor());
    }

    QSize newSize = webView->page()->mainFrame()->contentsSize();
    if (contentsSize != newSize || force)
    {
        contentsSize = newSize;
        resize(newSize);
    }
}

void KWindow::setOpacity(int a)
{
    alpha = a;
    setWindowOpacity((double)a / 255);
}

int KWindow::opacity()
{
    return alpha;
}

int KWindow::windowLevel()
{
    return windowZ;
}

void KWindow::setSnapToScreen(bool snapTo)
{
    snap = snapTo;
}

bool KWindow::isSnapToScreen()
{
    return snap;
}

void KWindow::snapToScreen()
{
    QDesktopWidget desktop;
    QRect screenRect = desktop.screenGeometry(this);

    int x = this->x();
    int y = this->y();
    int dx = x;
    int dy = y;
    int ddx = 0;
    int ddy = 0;

    if (x < screenRect.x())
        dx = screenRect.x();
    else if (x + width() > screenRect.x() + screenRect.width())
        dx = screenRect.x() + screenRect.width() - width();
    if (y < screenRect.y())
        dy = screenRect.y();
    else if (y + height() > screenRect.y() + screenRect.height())
        dy = screenRect.y() + screenRect.height() - height();

    if (dx != x)
    {
        ddx = (dx - x) >> 1;
        move(x + ddx, y);
    }
    if (dy != y)
    {
        ddy = (dy - y) >> 1;
        move(x + ddx, y + ddy);
    }
}

double KWindow::zoomFactor()
{
    return zoom;
}

void KWindow::setZoomFactor(double f)
{
    if (f < 0.5)
        f = 0.5;
    if (f > 2)
        f = 2.5;
    zoom = f;
}

void KWindow::setupWindowProperties()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    QPalette pal = palette();
    pal.setBrush(QPalette::Base, Qt::transparent);
    webView->setPalette(pal);

    if (1)
    {
        setAttribute(Qt::WA_TranslucentBackground, true);
    }

    webView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    webView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    webView->setBuffered(true);
}
