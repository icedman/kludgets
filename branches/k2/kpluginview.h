#ifndef KPLUGINVIEW_H
#define KPLUGINVIEW_H

#include <QWidget>
#include <QTimer>
#include <QTime>

class KWindow;
class KView;

class KPluginView : public QWidget
{
    Q_OBJECT
public:

    KPluginView(KWindow *window, KView *webview);
    void closeEvent(QCloseEvent *ev);

    QRegion region();

    static bool findAndHostPlugins(KWindow *window, KView *webview);

    private
Q_SLOTS:
    void onUpdate();
    void onParentDestroyed();

private:
    KWindow *parent;
    KView *view;
    QTimer updateTimer;
    QTime lastUpdate;
    int windowId;

};

#endif // KPLUGINVIEW_H
