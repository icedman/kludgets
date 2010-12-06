#include "disk.h"
#include "perfmon.h"

#include <windows.h>
#include <tchar.h>

DataMinerDisk::DataMinerDisk(QObject *parent)
        : QObject(parent),
        perf(new CPerfMon),
        diskCount(0)
{
    setObjectName("disk");
	perf->Initialize();
    // update();
}

DataMinerDisk::~DataMinerDisk()
{
    if (perf)
    {
        perf->Uninitialize();
        delete perf;
    }
}

void DataMinerDisk::enumerateDisks()
{
    int counterIndex[26];
    for (int i = 0; i < 26; i++)
    {
        TCHAR objName[255];
        _stprintf(objName, L"\\LogicalDisk(%c:)\\Free Megabytes", i + 64);
        counterIndex[i] = perf->AddCounter(objName);
    }

    perf->CollectQueryData();

    // validate
    for (int i = 0; i < 26; i++)
    {
        int val = perf->GetCounterValue(counterIndex[i]);
        if (val > 0)
        {
            TCHAR path[4];
            _stprintf(path, L"%c:\\", i + 64);
            QString pathName = QString::fromWCharArray(path);

            TCHAR name[255] = L"";
            GetVolumeInformation(path, name, 255, 0, 0, 0, 0, 0);

            QString driveName = QString::fromWCharArray(name);
            if (driveName == "")
                driveName = pathName;

            Data *data = &disks[diskCount];
            data->insert("freeMbCounter", counterIndex[i]);

            TCHAR objName[255];
            _stprintf(objName, L"\\LogicalDisk(%c:)\\%c Free Space", i + 64, '%');
            data->insert("freeSpaceCounter", perf->AddCounter(objName));

            data->insert("name", driveName);
            data->insert("path", pathName);
            data->insert("icon", "");

            data->setValid(true);
            diskCount++;
        }
        else
        {
            perf->RemoveCounter(counterIndex[i]);
        }
    }
}

int DataMinerDisk::getDiskCount()
{
    return diskCount;
}

QObject* DataMinerDisk::getDisk(int id)
{
    if (id > 25)
        id = 0;

    Data *data = &disks[id];
    if (data->isValid())
    {
        int freeMB = perf->GetCounterValue(data->value("freeMbCounter").toInt());
        int freeSpace = perf->GetCounterValue(data->value("freeSpaceCounter").toInt());
        data->insert("usage", freeSpace);
        data->insert("total", 0); // isn't used anyway
        data->insert("free", convertToBytesString((double)freeMB * 1024 * 1024));
    }

    return (QObject*)data;
}

void DataMinerDisk::update()
{
    if (!diskCount)
        enumerateDisks();

    if (perf)
        perf->CollectQueryData();
}
