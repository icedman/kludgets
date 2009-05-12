#ifndef KLUDGETCLIENT_H
#define KLUDGETCLIENT_H

#include <QtCore>
#include <QStringList>
#include <QSharedMemory>

#include "localefilelist.h"
#include "kludgetinfo.h"

class Kludget;
typedef QList<Kludget*> KludgetList;

class KClient : public QObject
{
    Q_OBJECT

public:

    static KClient* instance();
    static bool installPackage(const QString& path);

    bool initialize(const QString& path);
    void shutdown();

    Kludget* createInstance(const QString &instance = "");

private:
    bool installWidget(const QString& path, bool prompt = true);
    bool installFromArchive(const QString& path, bool prompt = true);
    bool installFromDirectory(const QString& path, bool prompt = true);
    void loadCustomFonts(const QString& path);
    void loadPlugins(const QString& path);

    private
Q_SLOTS:
    bool run();
    bool processInstanceQueue();
    void kludgetDestroyed(QObject *k);
    void exit();

private:

    KludgetList kludgets;
    KludgetInfo info;
    QStringList instanceQueue;
    LocaleFileList localefiles;

    QSharedMemory *processLock;
};

#endif // KLUDGETCLIENT_H
