#include "kipc.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QProcess>

void KIPC::closeProcess(int pid)
{
    if (checkProcess(pid))
    {
        QProcess process;
        QString cmd = QString("kill ") + QString::number(pid);
        process.start(cmd, QIODevice::ReadOnly);
        process.waitForStarted(1000);
        process.waitForFinished(3000);
    }
}

bool KIPC::checkProcess(int pid)
{
    QProcess process;
    QString cmd = QString("ps ") + QString::number(pid);
    process.start(cmd, QIODevice::ReadOnly);
    process.waitForStarted(1000);
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();

    if (output.contains(QString::number(pid)) && output.contains("Kludgets"))
    {
        return true;
    }

    return false;
}
