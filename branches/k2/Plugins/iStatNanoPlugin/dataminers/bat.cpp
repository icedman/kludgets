#include "bat.h"
#include "data.h"
#include "perfmon.h"

#include <QStringList>

#include <windows.h>
#include <setupapi.h>
#include <batclass.h>
#include <devguid.h>

DataMinerBat::DataMinerBat(QObject *parent)
        : QObject(parent),
        perf(new CPerfMon),
        batCount(0)
{
    setObjectName("bat");
    perf->Initialize();
}

DataMinerBat::~DataMinerBat()
{
    if (perf)
    {
        perf->Uninitialize();
        delete perf;
    }
}

void DataMinerBat::addBattery(QString bat)
{
    QString name = bat;
    name.replace("(", "");
    name.replace(")", "");

    for (int i = 0; i < 8; i++)
    {
        Data *existing = &batteries[i];
        if (existing->value("name") == bat)
            return ;
    }

    int id = batCount++;
    Data *data = &batteries[id];
    data->insert("name", bat);
    data->insert("cycles", 0);
    getBattery(id);
}

int DataMinerBat::getBatteryCount()
{
    return batCount;
}

QObject* DataMinerBat::getBattery(int id)
{
    if (id > batCount)
        id = 0;

    Data *data = &batteries[id];

    QString bat = data->value("name").toString();

    TCHAR objName[255];
    QString tObject;
    int counter;

    if (!(data->value("remainCounter").toInt() > 0))
    {
        tObject = "\\BatteryStatus" + bat + "\\RemainingCapacity";
        memset(objName, 0, sizeof(TCHAR) * 255);
        tObject.toWCharArray(objName);
        counter = perf->AddCounter(objName);
        data->insert("remainCounter", counter);

        qDebug("try 1 %s", qPrintable(tObject));
    }

    if (!(data->value("dischargeCounter").toInt() > 0))
    {
        tObject = "\\BatteryStatus" + bat + "\\DischargeRate";
        memset(objName, 0, sizeof(TCHAR) * 255);
        tObject.toWCharArray(objName);
        counter = perf->AddCounter(objName);
        data->insert("dischargeCounter", counter);

        qDebug("try 2 %s", qPrintable(tObject));
    }

    if (!(data->value("chargeCounter").toInt() > 0))
    {
        tObject = "\\BatteryStatus" + bat + "\\ChargeRate";
        memset(objName, 0, sizeof(TCHAR) * 255);
        tObject.toWCharArray(objName);
        counter = perf->AddCounter(objName);
        data->insert("chargeCounter", counter);

        qDebug("try 3 %s", qPrintable(tObject));
    }

    long charge = perf->GetCounterValue(data->value("chargeCounter").toInt());
    long discharge = perf->GetCounterValue(data->value("dischargeCounter").toInt());
    long remain = perf->GetCounterValue(data->value("remainCounter").toInt());
    long capacity = data->value("capacity").toInt();
    long life = 100;

    if (!capacity || remain > capacity)
    {
        capacity = remain;
        data->insert("capacity", capacity);
    }

    QString time;

    if (capacity > 0 && remain > 0)
        life = remain * 100 / capacity;

    if (discharge == BATTERY_UNKNOWN_RATE)
        discharge = 0;

    if (discharge > 0)
    {
        time = QString::number((double)capacity / discharge, 'f', 2) + " hrs";
    }

    data->insert("remaining", remain);
    data->insert("time", time);
    data->insert("life", life);
    data->insert("charge", charge);
    data->insert("discharge", discharge);

    return (QObject*)data;
}

void DataMinerBat::update()
{
    perf->CollectQueryData();
}
