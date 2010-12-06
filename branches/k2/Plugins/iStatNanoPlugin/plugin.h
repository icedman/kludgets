#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>

class QWebView;
class QWebFrame;
class DataMinerCpu;
class DataMinerDisk;
class DataMinerMem;
class DataMinerNet;
class DataMinerBat;

class iStatNanoPlugin : public QObject
{
    Q_OBJECT

public:
    iStatNanoPlugin();
    ~iStatNanoPlugin();

    void initWithWebView(QWebView *);
    void windowScriptObjectAvailable(QWebFrame *);

private:
    QString loadScript(const QString &path);

    public
Q_SLOTS:

    bool isIntel();
    bool needsIntelBundle();
    void setNeedsSMARTUpdate();
    void setShouldMonitorSMARTTemps(int i);
    QString uptime();

public:
    DataMinerBat *bat;
    DataMinerCpu *cpu;
    DataMinerDisk *disk;
    DataMinerMem *memory;
    DataMinerNet *net;

private:
    QWebView *view;

    bool smartMonitoringEnabled;
    bool shouldUpdateSMART;

    int uptimeCounter;

};

#endif // PLUGIN_H
