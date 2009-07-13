#ifndef KLUDGETVIEW_H
#define KLUDGETVIEW_H

#include <QObject>
#include <QTimer>
#include <QTimeLine>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>

class KPluginView;
class KWindow;
class KView : public QWebView
{
    Q_OBJECT

public:
    KView(KWindow *parent);

    enum RenderLayer {
        Background,
        Foreground,
        Transition
    };

    enum Transition {
        ToFront,
        ToBack,
        FirstShow
    };

    void screenshot(const QString &path);
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *e);
    void contextMenuEvent(QContextMenuEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

    void paintToBuffer(QImage *buf = 0);
    bool isBuffered();
    void setBuffered(bool);
    void setupBuffer();
    void renderLayer(int z);
    void setTransitionLayer(const QImage pix);

    void setFrozen(bool);
    bool isFrozen();
    void setGrayed(bool);
    bool isGrayed();
    void setTinted(bool);
    bool isTinted();
    void setTintColor(QColor c);
    QColor tintColor();
    void setTintMode(int mode = 14);
    int tintMode();

    void setTransition(int);
    void updateTransition();
    bool isTransitioning();

public Q_SLOTS:
    bool beginTransition();
    void endTransition();

    QImage &bufferImage();

private:

    KWindow *parent;

    int transition;
    QTimeLine *transitionTL;

    bool frozen;
    bool grayed;
    bool tinted;
    QColor tint;
    int tintCompositionMode;

    QImage buffer;
    bool buffered;
    bool dirtyBuffer;
    QTimer updateTimer;
    QImage layers[3];

    QPoint lastPos;
    QPoint dragStartPos;
    bool leftButtonPressed;

private Q_SLOTS:
    void onUpdate();

Q_SIGNALS:
    void bufferUpdated();
    void contextMenuRequested();
    void urlReceived(const QUrl*);

};

#endif // KLUDGETVIEW_H
