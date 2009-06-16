#include "kipc.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>

int KIPC::getProcessId(QString kludgetId)
{
    int pid = 0;
    QString pidfile = QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation)).absolutePath() + "/" + kludgetId + ".pid";
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

    qDebug(">>%s %d", qPrintable(kludgetId), pid);

    if (!checkProcess(pid))
    {
        return 0;
    }

    return pid;
}
