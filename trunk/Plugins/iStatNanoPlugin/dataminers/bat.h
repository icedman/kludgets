#ifndef DATAMINER_BAT_H
#define DATAMINER_BAT_H

#include <QObject>
#include "data.h"

class CPerfMon;
class DataMinerBat : public QObject
{
    Q_OBJECT
public:
    DataMinerBat(QObject *parent);
    ~DataMinerBat();

    public
Q_SLOTS:
    void addBattery(QString bat);
    int getBatteryCount();
    QObject* getBattery(int id);
    void update();

private:
    void enumerateBatteries();

private:
    Data batteries[8];

    int batCount;
    CPerfMon *perf;
};

#endif // DATAMINER_BAT_H
