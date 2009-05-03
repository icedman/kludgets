#include "config.h"
#include "ksettings.h"
#include "kdocument.h"

#include <QSettings>
#include <QtXml>

bool readXmlNode(QDomNode &node, QSettings::SettingsMap &map)
{
    QDomNode n = node;
    while (!n.isNull())
    {
        if (n.nodeType() == QDomNode::NodeType::TextNode)
        {
            QString path = KDocument::buildNodePath(n);
            if (!path.isEmpty())
            {
                map.insert(path, n.nodeValue());
            }
        }

        QDomElement elm = n.toElement();
        if (elm.hasAttributes())
        {
            QDomNamedNodeMap attribs = elm.attributes();
            for (int i = 0; i < attribs.length(); i++)
            {
                QDomNode attrib = attribs.item(i);
                QString path = KDocument::buildNodePath(attrib);
                map.insert(path, attrib.nodeValue());
                //qDebug("%s = %s", qPrintable(path), qPrintable(attrib.nodeValue()));
            }
        }

        if (n.hasChildNodes())
        {
            readXmlNode(n.firstChild(), map);
        }

        n = n.nextSibling();
    }
    return true;
}

bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QDomDocument dom;
    QString content = device.readAll();
    dom.setContent(content.trimmed());
    readXmlNode(dom.documentElement(), map);
    return true;
}

bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    KDocument dom;
    QSettings::SettingsMap::iterator it = map.begin();
    while (it != map.end())
    {
        if (!it.value().toString().isEmpty())
        {
            dom.setValue(it.key(), it.value().toString());
        }
        it++;
    }
    device.write(QString(XML_DECL).toUtf8());
    device.write(dom.toString().toUtf8());
    return true;
}

KSettings::KSettings(QObject* parent, const QString &path) :
        QObject(parent),
        settings(0),
        readOnly(false)
{
    setObjectName("Settings");
    if (path != "")
        setPath(path);
}

KSettings::~KSettings()
{
    if (settings)
    {
        delete settings;
    }
}

QVariant KSettings::read(const QString &key, QVariant defaultValue)
{
    if (!settings)
        return QVariant();

    if (key == "")
        return defaultValue;

    return settings->value(prependRootKey(key), defaultValue);
}

bool KSettings::write(const QString &key, QVariant value)
{
    if (key == "")
        return false;

    if (settings && !readOnly)
    {
        settings->setValue(prependRootKey(key), value);
        return true;
    }

    return false;
}

bool KSettings::contains(const QString &key)
{
    if (key == "")
        return false;

    if (settings)
        return settings->contains(prependRootKey(key));
    return false;
}

QString KSettings::path()
{
    return settingsPath;
}

void KSettings::setPath(const QString &p)
{
    if (!settings)
    {
        QFileInfo fileInfo(p);
        if (fileInfo.suffix() == "xml")
        {
            settings = new QSettings(p, QSettings::registerFormat("xml", readXmlFile, writeXmlFile));
        }
        if (!settings)
        {
            settings = new QSettings(p);
        }
    }
    settingsPath = p;
}

void KSettings::setRootKey(const QString &r)
{
    rootKey = r;
}

void KSettings::setReadOnly(bool r)
{
    readOnly = r;
}

QString KSettings::prependRootKey(const QString &k)
{
    QString key = k;
    if (rootKey != "")
    {
        if (k.indexOf(rootKey) != 0)
        {
            key = rootKey + "/" + k;
        }
    }
    return key.replace(" ", "");
}

void KSettings::sync()
{
    if (settings)
        settings->sync();
}

bool KSettings::isReadOnly()
{
    return readOnly;
}

void KSettings::clear()
{
    if (readOnly)
        return ;

    QFile::remove
        (settingsPath);
}
