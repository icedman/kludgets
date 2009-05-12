#include "klog.h"

#include <QApplication>
#include <QFile>

KLog* KLog::instance()
{
    static KLog log;
    return &log;
}

KLog::KLog()
{
    setPath(QApplication::applicationDirPath() + "/kludgets.log");
}

void KLog::setPath(const QString &p)
{
    path = p;
}

void KLog::clear()
{
    write("--------------", true);
    write("Kludget Engine");
    write("--------------");
}

void KLog::write(const QString &message, bool clear)
{
    QFile file;
    file.setFileName(path);
    if (!clear)
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        file.open(QIODevice::WriteOnly);
    file.write(message.toUtf8());
    file.write("\r\n", 2);
    file.close();
}
