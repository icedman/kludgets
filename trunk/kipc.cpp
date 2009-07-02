#include "kipc.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>

int KIPC::getProcessId(const QString &kludgetId)
{
    int pid = 0;
    QString pidfile = getPIDFile(kludgetId);
    if (QFile::exists(pidfile))
    {
        QFile file;
        file.setFileName(pidfile);
        if (file.open(QIODevice::ReadOnly))
        {
            QString text = file.readAll();
            file.close();
            pid = text.toInt();
        }
    }

    // qDebug(">>%s %d", qPrintable(kludgetId), pid);

    if (!checkProcess(pid))
    {
        return 0;
    }

    return pid;
}

bool KIPC::setProcessId(const QString &kludgetId, int pid)
{
    QString pidfile = getPIDFile(kludgetId);
    QFile file;
    file.setFileName(pidfile);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QString::number(pid).toUtf8());
        file.write("\r\n", 2);
        file.close();
        return true;
    }
    return false;
}

QString KIPC::getPIDFile(const QString &kludgetId)
{
    return QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation)).absolutePath() + "/" + kludgetId + ".kludget.pid";
}

void KIPC::destroyPIDFile(const QString &kludgetId)
{
    QString pidfile = getPIDFile(kludgetId);
    if (QFile::exists(pidfile))
        QFile::remove
            (pidfile);

}
