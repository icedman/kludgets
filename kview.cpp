
#include "config.h"
#include "kwindow.h"
#include "kview.h"
#include "kpage.h"
#include "klog.h"
#include "kpluginview.h"
#include "kutil.h"

#define ENABLE_DASHBOARD_SUPPORT 0
#define ENABLE_TRANSITION 0

KView::KView(KWindow *p) :
        QWebView(p),
        parent(p),
        updateTimer(this),
        dirtyBuffer(false),
        grayed(false),
        tinted(false),
        frozen(false),
        leftButtonPressed(false),
        transitionTL(0)
{
    setPage(new KPage(this));
    setBuffered(false);

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));

    updateTimer.start(100);
}

void KView::resizeEvent(QResizeEvent *e)
{
    QWebView::resizeEvent(e);
    setupBuffer();
}

void KView::paintEvent(QPaintEvent *ev)
{
    if (!parent->isVisible())
    {
        ev->accept();
        return ;
    }

    if (isBuffered())
    {
        if (isTransitioning())
        {
            updateTransition();
        }
        else
        {
            while (KPluginView::findAndHostPlugins(parent, this))
                {}
            paintToBuffer();
        }

        QPainter p;
        p.begin(this);
        p.drawImage(0, 0, buffer);

#if 0

        QImage blurred = buffer;
        ImageUtil::blur(blurred, 1);
        p.setOpacity(0.4);
        p.drawImage(0, 0, blurred);
#endif

        p.end();

        return ;
    }

    QWebView::paintEvent(ev);
}

void KView::contextMenuEvent(QContextMenuEvent* ev)
{
    if (isFrozen())
        return ;

    ev->accept();
    emit contextMenuRequested();
}

void KView::mouseMoveEvent(QMouseEvent* ev)
{
    if (isFrozen())
        return ;

    if (parent->isDragging())
    {
        ev->accept();
        return ;
    }
    else
    {
        if (leftButtonPressed)
        {
            QPoint p = dragStartPos - ev->pos();
            if (p.manhattanLength() > 4)
            {
                p = dragStartPos + parent->pos();
                parent->startDrag(p.x(), p.y());
            }
        }
    }

    QWebView::mouseMoveEvent(ev);
}

void KView::mousePressEvent(QMouseEvent *ev)
{
    if (isFrozen())
        return ;

    bool allowDrag = true;

#if ENABLE_DASHBOARD_SUPPORT

    allowDrag = !page()->dashboardRegionContains(ev->pos());
#else

    allowDrag = ev->modifiers() & Qt::ControlModifier;
#endif

    if (ev->button() == Qt::RightButton)
        return ;

    if (allowDrag)
    {
        if (ev->button() == Qt::LeftButton)
        {
            leftButtonPressed = true;
            dragStartPos = ev->pos();
            return ;
        }
    }
    else
    {
        QWebView::mousePressEvent(ev);
    }

}

void KView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (parent->isDragging())
    {
        parent->endDrag();
    }
    else if (leftButtonPressed)
    {
        QMouseEvent press(
            QEvent::MouseButtonPress,
            ev->pos(),
            ev->globalPos(),
            Qt::LeftButton,
            0,
            ev->modifiers());
        QWebView::mousePressEvent(&press);
    }

    leftButtonPressed = false;

    if (isFrozen())
        return ;

    /*
    // this breaks the menu on windows 7
    if (ev->button() == Qt::RightButton)
    {
        //emit contextMenuRequested();
        return ;
    }
    */

    QWebView::mouseReleaseEvent(ev);
}

void KView::mouseDoubleClickEvent(QMouseEvent* ev)
{
    if (isFrozen())
        return ;

    QWebView::mouseDoubleClickEvent(ev);
}

void KView::focusInEvent(QFocusEvent *ev)
{
    QWebView::focusInEvent(ev);

    if (parent->windowLevel() == 2)
        parent->lower();
}

void KView::focusOutEvent(QFocusEvent *ev)
{
    QWebView::focusOutEvent(ev);

    leftButtonPressed = false;
}

void KView::dragEnterEvent(QDragEnterEvent *ev)
{
    const QMimeData *mime = ev->mimeData();
    if (mime->hasUrls())
    {
        ev->acceptProposedAction();
    }
}

void KView::dropEvent(QDropEvent *ev)
{
    if (!ev->mimeData())
        return ;

    const QMimeData *mime = ev->mimeData();
    if (mime->hasUrls())
    {
        QList<QUrl> urls = mime->urls();
        for (int i = 0; i < urls.size(); i++)
        {
            emit urlReceived(&urls.at(i));
            //qDebug("drop url: %s", qPrintable(urls.at(i).toLocalFile()));
        }
    }
}

bool KView::isBuffered()
{
    return buffered;
}

void KView::setBuffered(bool b)
{
    buffered = b;
    if (b)
    {
        setupBuffer();
    }
}

void KView::setupBuffer()
{
    if (isBuffered())
    {
        buffer = QImage(width(), height(), QImage::Format_RGB32);
        buffer.fill(Qt::transparent);
        update();
    }
}

void KView::setFrozen(bool f)
{
    frozen = f;
}

bool KView::isFrozen()
{
    return frozen;
}

void KView::setGrayed(bool g)
{
    grayed = g;
}

bool KView::isGrayed()
{
    return grayed;
}

void KView::setTinted(bool t)
{
    tinted = t;
}

bool KView::isTinted()
{
    return tinted;
}

void KView::setTintMode(int mode)
{
    tintCompositionMode = mode;
}

int KView::tintMode()
{
    return tintCompositionMode;
}

void KView::setTintColor(QColor c)
{
    tint = c;
}

QColor KView::tintColor()
{
    return tint;
}

void KView::screenshot(const QString &path)
{
    if (isBuffered())
    {
        KLog::log(QString("screenshot:") + path);
        QImage img(buffer.width(), buffer.height(), QImage::Format_ARGB32);
        img.fill(Qt::white);
        QPainter p(&img);
        p.drawImage(0,0,buffer);
        img.save(path);
    }
}

void KView::paintToBuffer(QImage *buf)
{
    QWebFrame *frame = page()->mainFrame();
    if (!frame)
        return ;

    if (!isBuffered())
        return ;

    if (!buf)
        buf = &buffer;

    QPainter p;

    p.begin(buf);

    // clear
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(geometry(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // background
    if (buf == &buffer)
    {
        p.drawImage(0, 0, layers[0]);
    }

    // body
    frame->render(&p);

    // foreground
    if (buf == &buffer)
    {
        p.drawImage(0, 0, layers[1]);
    }

    p.end();

    if (buf == &buffer)
    {
        // apply image filter
        if (isGrayed())
        {
            buffer = ImageUtil::grayedImage(buffer);
        }
        else if (isTinted())
        {
            buffer = ImageUtil::tintedImage(buffer, tintColor(), (QPainter::CompositionMode)tintMode());
        }
    }
}

void KView::renderLayer(int z)
{
    if (z < 0 || z > KView::Transition)
        return ;

    if (layers[z].width() != width() && layers[z].height() != height())
    {
        layers[z] = QImage(width(), height(), QImage::Format_RGB32);
        layers[z].fill(Qt::transparent);
    }

    paintToBuffer(&layers[z]);
}

void KView::setTransitionLayer(const QImage pix)
{
    layers[Transition] = pix;
}

QImage& KView::bufferImage()
{
    return buffer;
}

void KView::onUpdate()
{
    if (!parent->isVisible())
        return ;

    QPoint cursorPos = QCursor::pos();
    QRect frameGeometry = parent->frameGeometry();
    if (frameGeometry.contains(lastPos) && !frameGeometry.contains(cursorPos))
    {
        page()->mainFrame()->evaluateJavaScript("Kludget.dispatchMouseLeave()");
    }

    lastPos = cursorPos;
}

void KView::setTransition(int t)
{
    transition = t;
}

bool KView::beginTransition()
{
    // qDebug("beginTransition");

    int duration = 1000;

    transitionTL = new QTimeLine(duration, this);
    connect(transitionTL, SIGNAL(finished()), this, SLOT(endTransition()));
    transitionTL->setStartFrame(1);
    transitionTL->setEndFrame(25);

    switch (transition)
    {
    case ToFront:
    case ToBack:
        break;
    case FirstShow:
        duration = 1200;
        break;
    }

#if !ENABLE_TRANSITION

    duration = 600;
    hide();
#endif

    transitionTL->setDuration(duration);
    transitionTL->start();

    return true;
}

void KView::endTransition()
{
    //qDebug("endTransition");

    if (transitionTL)
    {
        delete transitionTL;
        transitionTL = 0;
    }

    setFrozen(false);

#if !ENABLE_TRANSITION

    show();
#else

    emit bufferUpdated();
#endif
}

void KView::updateTransition()
{
#if !ENABLE_TRANSITION
    return ;
#endif

    if (!transitionTL)
        return ;

    int transitionFrame = transitionTL->currentFrame();
    int transitionLength = transitionTL->endFrame();

    paintToBuffer();

    double a1 = (double)transitionFrame / transitionLength;
    double a2 = 1 - ((double)transitionFrame / transitionLength);

    buffer = ImageUtil::blendImages(buffer, a1, layers[2], a2);

    emit bufferUpdated();
}

bool KView::isTransitioning()
{
    return (transitionTL != 0);
}
