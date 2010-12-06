#include "cpu.h"
#include "data.h"
#include "perfmon.h"

#include <windows.h>

DataMinerCpu::DataMinerCpu(QObject *parent)
        : QObject(parent),
        perf(0),
        data(new Data(this))
{
    setObjectName("cpu");
}

DataMinerCpu::~DataMinerCpu()
{
    if (perf)
    {
        perf->Uninitialize();
        delete perf;
    }
}

QObject* DataMinerCpu::getUsage()
{
    if (!perf)
    {
        perf = new CPerfMon;
        perf->Initialize();
        load = perf->AddCounter(L"\\Processor(_Total)\\% Privileged Time");
        user = perf->AddCounter(L"\\Processor(_Total)\\% User Time");

        load1 = perf->AddCounter(L"\\Processor(0)\\% Privileged Time");
        user1 = perf->AddCounter(L"\\Processor(0)\\% User Time");
        load2 = perf->AddCounter(L"\\Processor(1)\\% Privileged Time");
        user2 = perf->AddCounter(L"\\Processor(1)\\% User Time");
    }

    perf->CollectQueryData();

    if (perf->GetCounterValue(load2) > 0)
    {
        data->insert("system1", perf->GetCounterValue(load1));
        data->insert("user1", perf->GetCounterValue(user1));
        data->insert("system2", perf->GetCounterValue(load2));
        data->insert("user2", perf->GetCounterValue(user2));
    }

    data->insert("system", perf->GetCounterValue(load));
    data->insert("user", perf->GetCounterValue(user));


    return (QObject*)data;
}

int DataMinerCpu::loadAverage()
{
    long max, min, mean;
    long maxUser, minUser, meanUser;
    perf->GetStatistics(&min, &max, &mean, load);
    perf->GetStatistics(&minUser, &maxUser, &meanUser, user);
    return (mean + meanUser) >> 1;
}
