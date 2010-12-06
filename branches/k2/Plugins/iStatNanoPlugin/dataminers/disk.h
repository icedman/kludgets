#ifndef DATAMINER_DISK_H
#define DATAMINER_DISK_H

#include <QObject>
#include <QStringList>

#include "data.h"

class CPerfMon;
class DataMinerDisk : public QObject
{
    Q_OBJECT
public:
    DataMinerDisk(QObject *parent);
    ~DataMinerDisk();

private:

    void enumerateDisks();

    public
Q_SLOTS:
    void update();
    QObject* getDisk(int id);
    int getDiskCount();

private:
    Data disks[26];
    int diskCount;
    CPerfMon *perf;
};

#endif // DATAMINER_CPU_H
