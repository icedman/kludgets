#ifndef KLUDGETINFO_H
#define KLUDGETINFO_H

#include <QString>
#include <QList>

class KludgetInfo
{
public:

    KludgetInfo();
    KludgetInfo(const QString &path, const QString &instance);

    void read();
    bool isValid();

    QString path;
    QString storagePath;
    QString configFile;
    QString preferenceFile;
    QString instancePreferenceFile;

    QString id;
    QString instance;
    QString name;

    bool debug;
    int width;
    int height;
    QString contentSrc;
};

#endif // KLUDGETINFO_H
