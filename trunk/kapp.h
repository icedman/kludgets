#ifndef KLUDGETSAPPLICATION_H
#define KLUDGETSAPPLICATION_H

#include <QApplication>

class KApp : public QApplication
{
public:
    KApp(int argc, char *argv[]);

    bool startClient(const QString &path);
    bool startServer();
    bool startHotKeyListener();
};

#endif // KLUDGETSAPPLICATION_H
