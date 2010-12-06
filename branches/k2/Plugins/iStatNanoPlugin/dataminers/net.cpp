#include "net.h"
#include "data.h"
#include "perfmon.h"

#include <windows.h>

DataMinerNet::DataMinerNet(QObject *parent)
        : QObject(parent),
        perf(0),
        netCount(0),
        elapsed(0)
{
    setObjectName("net");
}

DataMinerNet::~DataMinerNet()
{
    if (perf)
    {
        perf->Uninitialize();
        delete perf;
    }
}

void DataMinerNet::addInterface(QString net)
{
    // oops
    if (netCount == 8)
        return ;

    if (!perf)
    {
        perf = new CPerfMon;
        perf->Initialize();
    }

    QString name = net;
    name.replace("(", "");
    name.replace(")", "");

    for (int i = 0; i < 8; i++)
    {
        Data *existing = &interfaces[i];
        if (existing->value("name") == name)
            return ;
    }

    Data *data = &interfaces[netCount++];
    data->insert("name", name);

    QString tSent = "\\Network Interface" + net + "\\Bytes Sent/sec";
    QString tReceived = "\\Network Interface" + net + "\\Bytes Received/sec";
    TCHAR objName[255];

    memset(objName, 0, sizeof(TCHAR) * 255);
    tSent.toWCharArray(objName);
    data->insert("sentCounter", perf->AddCounter(objName));

    memset(objName, 0, sizeof(TCHAR) * 255);
    tReceived.toWCharArray(objName);
    data->insert("receivedCounter", perf->AddCounter(objName));

    data->insert("totalSentRaw", 0);
    data->insert("totalReceivedRaw", 0);
}

int DataMinerNet::getInterfaceCount()
{
    return netCount;
}

QObject* DataMinerNet::getInterface(int id)
{
    if (id > netCount)
        id = 0;

    Data *data = &interfaces[id];

    if (perf)
    {
        double e = (double)elapsed / 1000;
        int sent = perf->GetCounterValue(data->value("sentCounter").toInt()) * e;
        int received = perf->GetCounterValue(data->value("receivedCounter").toInt()) * e;

        int totalSent = data->value("totalSentRaw").toInt() + sent;
        int totatReceived = data->value("totalReceivedRaw").toInt() + received;

        data->insert("sent", convertToBytesString(sent).toLower());
        data->insert("received", convertToBytesString(received).toLower());
        data->insert("totalSent", convertToBytesString(totalSent).toLower());
        data->insert("totalReceived", convertToBytesString(totatReceived).toLower());
        data->insert("totalSentRaw", totalSent);
        data->insert("totalReceivedRaw", totatReceived);
    }

    return (QObject*)data;
}

void DataMinerNet::update()
{
    if (timer.isNull())
    {
        timer.start();
    }

    elapsed = timer.elapsed();
    timer.restart();

    if (perf)
        perf->CollectQueryData();
}
