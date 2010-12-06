#ifndef DATAMINER_CPU_H
#define DATAMINER_CPU_H

#include <QObject>

class CPerfMon;
class Data;
class DataMinerCpu : public QObject
{
    Q_OBJECT
public:
    DataMinerCpu(QObject *parent);
    ~DataMinerCpu();

    public
Q_SLOTS:
    QObject* getUsage();
    int loadAverage();

private:
    Data* data;
    int load;
    int user;

    // cpu1
    int load1;
    int user1;
    ;

    // cpu2
    int load2;
    int user2;

    CPerfMon *perf;
};

#endif // DATAMINER_CPU_H
