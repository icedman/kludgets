#ifndef DATAMINER_NET_H
#define DATAMINER_NET_H

#include <QObject>
#include <QTime>

#include "data.h"

class CPerfMon;
class DataMinerNet : public QObject
{
    Q_OBJECT
public:
    DataMinerNet(QObject *parent);
    ~DataMinerNet();

    public
Q_SLOTS:
    void addInterface(QString net);
    int getInterfaceCount();
    QObject* getInterface(int id);
    void update();

private:
    Data interfaces[8];

    int netCount;
    CPerfMon *perf;

    QTime timer;
    int elapsed;
};

#endif // DATAMINER_NET_H
