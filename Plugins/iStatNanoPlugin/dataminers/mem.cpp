#include "mem.h"
#include <windows.h>

DataMinerMem::DataMinerMem(QObject *parent)
        : QObject(parent),
        data(new Data(this))
{
    setObjectName("memory");
    update();
}

void DataMinerMem::update()
{
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&memStatus);
    data->insert("load", (long)memStatus.dwMemoryLoad);
    data->insert("free", convertToBytesString(memStatus.ullAvailPhys).toLower());
    data->insert("total", convertToBytesString(memStatus.ullTotalPhys).toLower());
    data->insert("used", convertToBytesString(memStatus.ullTotalPhys - memStatus.ullAvailPhys).toLower());
    data->insert("paged", convertToBytesString(memStatus.ullTotalPageFile - memStatus.ullAvailPageFile).toLower());
    // data->insert("nonpaged", convertToBytesString(memStatus.ullAvailPageFile).toLower());
    data->insert("totalVirtual", convertToBytesString(memStatus.ullTotalVirtual).toLower());
    data->insert("freeVirtual", convertToBytesString(memStatus.ullAvailVirtual).toLower());
    data->insert("usedVirtual", convertToBytesString(memStatus.ullTotalVirtual - memStatus.ullAvailVirtual).toLower());
    data->insert("totalPage", convertToBytesString(memStatus.ullTotalPageFile).toLower());

    /*
    0, // free
    1, // used
    2, // active
    3, // inactive
    4, // wired
    5, // percentage
    6, // pagein
    7, // pageouts
    8, // inactive + free
    9, // swap
    10, // wired %
    11, // active %
    12, // retain?
    */
}

QObject* DataMinerMem::getUsage()
{
    return (QObject*)data;
}

