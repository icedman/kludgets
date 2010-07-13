#ifndef KLUDGETWINDOW_H
#define KLUDGETWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QTimer>

class KView;

class KWindow : public QMainWindow
{
    Q_OBJECT

public:

    KWindow();

public:

    KView *view();

    bool isDragging();
    void startDrag(int x = 0, int y = 0);
    void endDrag();
    bool isIgnoreDrag();
    void setIgnoreDrag(bool);
    bool isIgnoreMouse();
    void setIgnoreMouse(bool);

    void setAutoSize(bool);
    bool isAutoSize();
    void autoSize(bool force = false);

    void setOpacity(int a);
    int opacity();
    int windowLevel();
    void setWindowLevel(int);

    void setSnapToScreen(bool);
    bool isSnapToScreen();
    void snapToScreen();

    double zoomFactor();
    void setZoomFactor(double f);

    void moveToTop();
    void moveToBottom();

    private
Q_SLOTS:
    void onGeometryChangeRequested(const QRect & rect);
    void onUpdate();

Q_SIGNALS:
    void onShow();
    void onHide();
    void onStartDrag();
    void onEndDrag();
    void onSettingsChanged();

private:
    void setupWindow();

private:

    KView *webView;
    QTimer updateTimer;

    bool dragging;
    bool noMouse;
    bool noDrag;
    QPoint dragPos;
    QSize contentsSize;
    int windowZ;
    int alpha;
    bool snap;
    bool autoSized;
    double zoom;

private:
    void updateWindowLevel(int);
    void updateMouseIgnore(bool);

#if 0
#if defined(WIN32)

    bool winEvent(MSG *message, long *result);
#else

    bool x11Event(XEvent * event);
#endif
#endif

};

#endif // KLUDGETWINDOW_H
