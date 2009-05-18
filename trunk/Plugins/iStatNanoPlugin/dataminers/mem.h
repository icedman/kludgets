#ifndef DATAMINER_MEM_H
#define DATAMINER_MEM_H

#include <QObject>
#include "data.h"

class DataMinerMem : public QObject
{
    Q_OBJECT
public:
    DataMinerMem(QObject *parent);


    public
Q_SLOTS:
    QObject* getUsage();
    void update();

private:
    Data* data;
};

#endif // DATAMINER_MEM_H
