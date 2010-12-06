#ifndef KLUDGETSAPPLICATION_H
#define KLUDGETSAPPLICATION_H

#include <QApplication>

class KApp : public QApplication
{
public:
    KApp(int argc, char *argv[]);

    bool startClient(const QString &path);
    bool startServer();
#if !defined(WIN32)

    bool startHotKeyListener();
#endif

    static QString temporaryDirPath();
    static QString userAgent();
    static QString version();
};

#endif // KLUDGETSAPPLICATION_H
