#include "klog.h"
#include "kapp.h"
#include "config.h"
#include "kdocument.h"

#include <QStandardPaths>
#include <QApplication>
#include <QFile>

KLog* KLog::instance()
{
    static KLog log;
    return &log;
}

KLog::KLog() : _enabled(true)
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
    write(KApp::userAgent());
    write("--------------");
}

void KLog::write(const QString &message, bool clear)
{
    qDebug("%s", qPrintable(message));

    if (!enabled())
        return ;

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

void KLog::setEnabled(bool e)
{
    _enabled = e;
}

bool KLog::enabled()
{
    return _enabled;
}

void KLog::loadSettings()
{
    KDocument doc;
    if (doc.openDocument(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + ENGINE_CONFIG_FILE))
    {
        if (doc.getValue("kludget/general/enableLog", "1").toInt() != 1)
        {
            setEnabled(false);
        }
    }
}

void KLog::log(const QString &message, bool clear)
{
    instance()->write(message, clear);
}
