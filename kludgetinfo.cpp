#include "config.h"
#include "kludgetinfo.h"
#include "kdocument.h"

#include <QApplication>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QUrl>

KludgetInfo::KludgetInfo()
{}

KludgetInfo::KludgetInfo(const QString &p, const QString &i)
{
    path = p;
    instance = i;
}

bool KludgetInfo::load()
{
    // local
    path = QDir(path).absolutePath();
    configFile = path + "/" + QString(CONFIG_FILE);

    if (!QFile(configFile).exists())
    {
        QString infoPlistFile = path + "/Info.plist";
        QString operaConfigXmlFile = path + "/config.xml";
        if (QFile(infoPlistFile).exists())
        {
            KDocument plist;
            plist.openDocument(infoPlistFile);
            plist.transform(":resources/xsl/infoPlist.xsl");
            plist.setValue("widget/engine", "dashboard");
            plist.saveDocument(configFile);
        }
        else if (QFile(operaConfigXmlFile).exists())
        {
            KDocument cfg;
            cfg.openDocument(operaConfigXmlFile);
            cfg.transform(":resources/xsl/operaConfig.xsl");
            cfg.setValue("widget/engine", "opera");
            cfg.saveDocument(configFile);
        }
        else
        {
            configFile = "";
            return false;
        }
    }

    QString defaultName = QDir(path).dirName();
    QString defaultId = defaultName;

    defaultId.replace(QRegExp("[ ]"), "_");
    defaultName.replace(QRegExp("[/_-]"), " ");

    KDocument doc;
    doc.openDocument(configFile);

    id = doc.getValue("widget/id", defaultId);
    name = doc.getValue("widget/name", defaultName);

    QUrl url = QUrl(doc.getValue("widget/content/src", ""));
    if (url.scheme() == "http")
        contentSrc = url.toString();
    else
        contentSrc = path + "/" + doc.getValue("widget/content/src", "index.html");
    contentHtml = doc.getValue("widget/content/html", "").trimmed();
    width = doc.getValue("widget/width", "0").toInt();
    height = doc.getValue("widget/height", "0").toInt();
    debug = (doc.getValue("widget/debug", "0").toInt() != 0);

    storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/widgets/") + id;
    instancePreferenceFile = storagePath + "/" + instance + "/" + QString(PREFERENCE_FILE);

    QString pluginName = doc.getValue("widget/plugins/plugin", "");

    if (pluginName != "")
    {
#ifdef Q_WS_MAC

        pluginPath = path + "/" + pluginName + "/Contents/MacOS";
#else
#if defined(WIN32)

        pluginPath = path + "/" + pluginName + "/Contents/Windows";
#endif
#endif

        // todo: read info.plist to check CFBundleExecutable
        pluginName.replace(".bundle", "");
        pluginExecutable = pluginName;
        pluginScript = pluginName + ".js";
    }

    return isValid();
}

bool KludgetInfo::isValid()
{
    return (configFile != "");
}
