#ifndef HUDWINDOW_H
#define HUDWINDOW_H

#include <QWidget>

class HudWindow : public QWidget
{
    Q_OBJECT
public:
    HudWindow();

    void mousePressEvent(QMouseEvent * event);
    void closeEvent(QCloseEvent * event);
};

#endif // HUDWINDOW_H
