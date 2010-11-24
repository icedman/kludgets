#include "plugin.h"
#include "dataminers/perfmon.h"
#include "dataminers/bat.h"
#include "dataminers/cpu.h"
#include "dataminers/disk.h"
#include "dataminers/mem.h"
#include "dataminers/net.h"

#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QFile>
#include <QTextStream>

#ifdef Q_WS_WIN
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

iStatNanoPlugin plugin;

extern "C" DLL_EXPORT void initWithWebView(QWebView *view)
{
    plugin.initWithWebView(view);
}

extern "C" DLL_EXPORT void windowScriptObjectAvailable(QWebFrame *frame)
{
    plugin.windowScriptObjectAvailable(frame);
}

iStatNanoPlugin::iStatNanoPlugin() :
        view(0),
        bat(new DataMinerBat(this)),
        cpu(new DataMinerCpu(this)),
        disk(new DataMinerDisk(this)),
        memory(new DataMinerMem(this)),
        net(new DataMinerNet(this)),
        smartMonitoringEnabled(false),
        shouldUpdateSMART(false)
{
    CPerfMon::instance()->Initialize();
    uptimeCounter = CPerfMon::instance()->AddCounter(L"\\System\\System Up Time");
}

iStatNanoPlugin::~iStatNanoPlugin()
{
    CPerfMon::instance()->Uninitialize();
}

void iStatNanoPlugin::initWithWebView(QWebView *v)
{
    view = v;
}

void iStatNanoPlugin::windowScriptObjectAvailable(QWebFrame *frame)
{
	qDebug("plugin added");
    frame->addToJavaScriptWindowObject("iStatNano", this);
    frame->evaluateJavaScript(loadScript(":plugin.js"));
}

QString iStatNanoPlugin::loadScript(const QString &path)
{
    QFile scriptFile(path);
    if (scriptFile.open(QIODevice::ReadOnly))
    {
        return QTextStream(&scriptFile).readAll();
    }
    return "";
}

bool iStatNanoPlugin::isIntel()
{
    return true;
}

bool iStatNanoPlugin::needsIntelBundle()
{
    return false;
}

void iStatNanoPlugin::setNeedsSMARTUpdate()
{
    shouldUpdateSMART = true;
}

void iStatNanoPlugin::setShouldMonitorSMARTTemps(int i)
{
    smartMonitoringEnabled = (i == 1);
}

QString iStatNanoPlugin::uptime()
{
    CPerfMon::instance()->CollectQueryData();
    long sec = CPerfMon::instance()->GetCounterValue(uptimeCounter);
    long hours = sec / 3600;
    long min = (sec - (hours * 3600)) / 60;
    QString t;
    t.sprintf("%d:%02d", hours, min);
    return t;
}
