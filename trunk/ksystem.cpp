#include "config.h"
#include "ksystem.h"
#include "kludget.h"
#include "kapp.h"
#include "klog.h"
#include "kutil.h"

#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QTemporaryFile>
#include <QCoreApplication>

KSystem::KSystem(QObject* parent) :
        QObject(parent),
        process(new QProcess(this)),
        pid(0),
        exitStatus(0),
        runInShell(false),
        runSystemCommands(false)
{
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
}

KSystem::~KSystem()
{
    if (QFile::exists(tempFile))
    {
        QFile::remove
        (tempFile);
    }
}

void KSystem::setShellPath(const QString path)
{
    runInShell = (path != "");
    shellPath = path;
}

void KSystem::setEnableCommands(bool enable)
{
    runSystemCommands = enable;
}

QString KSystem::version()
{
    return KApp::version();
}

void KSystem::log(QString message)
{
    KLog::log(message);
}

void KSystem::openURL(QString url)
{
    QDesktopServices::openUrl(QUrl::fromEncoded(url.toUtf8()));
}

int KSystem::execute(QString cmd)
{
    if (!runSystemCommands)
        return 0;

    QTemporaryFile tmpScript;

    // run inside cygwin bash
    if (runInShell)
    {
        if (tmpScript.open())
        {
            tmpScript.write(cmd.toUtf8());
            tmpScript.setAutoRemove(false);
            cmd = shellPath + " " + tmpScript.fileName();
        }
    }

    KSystem *k = new KSystem(this);
    k->tempFile = tmpScript.fileName();
    QProcess *p = k->process;
    p->start(cmd, QIODevice::ReadWrite);
    k->pid = (long)p->pid();
    processes.insert(k->pid, k);
    p->waitForStarted(1000);
    return k->pid;
}

void KSystem::close(long pid)
{
    KSystem * k = getProcess(pid);
    k->process->close();
}

void KSystem::write(QString string, long pid)
{
    KSystem *k = getProcess(pid);
    k->process->write(string.toUtf8());
}

void KSystem::cancel(long pid, int wait)
{
    qDebug("cancel");
    KSystem *k = getProcess(pid);
    QProcess *p = k->process;

    if (p->state() == QProcess::Running)
    {
        if (wait)
            p->waitForFinished(wait);
        p->terminate();
        p->close();
        k->readyReadStandardError();
        k->readyReadStandardOutput();
    }
}

void KSystem::readyReadStandardError()
{
    errorList.push_back(process->readAllStandardError());
    emit ((KSystem*)parent())->execUpdate(pid);
}

void KSystem::readyReadStandardOutput()
{
    outputList.push_back(process->readAllStandardOutput());
    emit ((KSystem*)parent())->execUpdate(pid);
}

void KSystem::finished(int code, QProcess::ExitStatus status)
{
    exitStatus = (int)status;
    emit ((KSystem*)parent())->execFinish(pid);
}

QString KSystem::error(long pid)
{
    // qDebug("error");
    KSystem *k = getProcess(pid);
    if (k->errorList.size() == 0)
        return "";
    QString res = k->errorList.first();
    k->errorList.pop_front();
    return res;
}

QString KSystem::output(long pid)
{
    // qDebug("output");
    KSystem *k = getProcess(pid);
    if (k->outputList.size() == 0)
        return "";
    QString res = k->outputList.first();
    k->outputList.pop_front();
    return res;
}

bool KSystem::hasError(long pid)
{
    // qDebug("hasError");
    KSystem *k = getProcess(pid);
    return k->errorList.size() > 0;
}

bool KSystem::hasOutput(long pid)
{
    // qDebug("hasOutput");
    KSystem *k = getProcess(pid);
    return k->outputList.size() > 0;
}

int KSystem::status(long pid)
{
    // qDebug("status");
    KSystem *k = getProcess(pid);
    return k->exitStatus;
}

KSystem *KSystem::getProcess(long pid)
{
    if (!processes.contains(pid))
    {
        // qDebug("not found %d!", pid);
        return this;
    }

    KSystem *sys = processes.value(pid);
    if (sys->process->state() == QProcess::NotRunning)
    {
        if (QFile::exists(sys->tempFile))
        {
            QFile::remove
            (sys->tempFile);
        }
    }

    // qDebug("found %d!", pid);
    return sys;
}

QString KSystem::encrypt(QString s)
{
    return Util::encrypt(s);
}

QString KSystem::decrypt(QString s)
{
    return Util::decrypt(s);
}