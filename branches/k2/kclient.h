#ifndef KLUDGETCLIENT_H
#define KLUDGETCLIENT_H

#include <QtCore>
#include <QStringList>

#include "klocalefilelist.h"
#include "kludgetinfo.h"

class Kludget;
typedef QList<Kludget*> KludgetList;

class KClient : public QObject
{
    Q_OBJECT

public:

    static KClient* instance();
    static bool registerPackage(const QString& path);

    bool initialize(const QString& path);
    void shutdown();

    Kludget* createInstance(const QString &instance = "");
    Kludget* getInstance(const QString &instance);

    void setEnabled(bool enabled);
    bool isEnabled();

private:
    bool loadPackage(const QString& path);
    void loadFonts(const QString& path);
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
};

#endif // KLUDGETCLIENT_H
