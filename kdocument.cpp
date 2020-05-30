#include "config.h"
#include "kdocument.h"
#include "kutil.h"

#include <QApplication>
#include <QTemporaryFile>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QFileInfo>
#include <QStringList>

#if defined(QT_XMLPATTERNS_LIB)
#include <QXmlQuery>
#endif

bool KDocument::openDocument(const QString &path)
{
    setContent(QString(""));

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("unable to open! %s", qPrintable(path));
        return false;
    }

    setContent(file.readAll().trimmed());
    file.close();
    return true;
}

bool KDocument::saveDocument(const QString &path)
{
    QFile file;
    file.setFileName(path);
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug("unable to save! %s", qPrintable(path));
        return false;
    }

    // file.write(QString(XML_DECL).toUtf8());
    file.write(toString().toUtf8());
    file.close();
    return true;
}

bool KDocument::transform(const QString &xslPath)
{
#if !defined(QT_XMLPATTERNS_LIB)
    QTemporaryFile tempXsl;
    QTemporaryFile tempContent;

    QFile file;
    file.setFileName(xslPath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    tempXsl.open();
    tempXsl.setAutoRemove(false);
    tempXsl.write(file.readAll());
    tempXsl.close();

    tempContent.open();
    tempContent.setAutoRemove(false);
    tempContent.write(toString().toUtf8());
    tempContent.close();

    QStringList args;
    args.push_back(tempXsl.fileName());
    args.push_back(tempContent.fileName());

    QString content = Util::execute("xmlpatterns", args);
    qDebug("%s\n", qPrintable(content));
    setContent(content);

    tempXsl.remove();
    tempContent.remove();

#else

    QFile xsl;
    xsl.setFileName(xslPath);
    if (!xsl.open(QIODevice::ReadOnly))
        return false;

    QBuffer focus;
    focus.open(QBuffer::ReadWrite);
    focus.write(toString().toUtf8());
    focus.seek(0);

    QString content;
    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(&focus);
    query.setQuery(&xsl);
    query.evaluateTo(&content);
    setContent(content);
#endif
    // qDebug("%s", qPrintable(content));
    return true;

}

bool KDocument::contains(const QString &nodePath)
{
    QString nullValue = ">doesNotHave" + nodePath + "<";
    QString tmp = getValue(nodePath, nullValue);
    return tmp != nullValue;
}

QString KDocument::getValue(const QString &nodePath, const QString &defaultValue)
{
    QString ret = defaultValue;

    QStringList nodeNames = nodePath.split("/");
    QDomNode n = documentElement();

    while (nodeNames.length()) //  && n)
    {
        QString targetNodeName = nodeNames.front();
        QString nodeName = n.nodeName();

        if (nodeName != targetNodeName)
        {
            // check attribute
            if (n.toElement().hasAttribute(targetNodeName))
            {
                ret = n.toElement().attribute(targetNodeName);
            }

            QDomNodeList nl = n.toElement().elementsByTagName(targetNodeName);
            if (!nl.size())
                break;

            n = nl.item(0);
            nodeName = n.nodeName();
        }

        nodeNames.pop_front();

        if (!nodeNames.length())
        {
            if (nodeName == targetNodeName)
            {
                ret = n.firstChild().nodeValue();
            }
            break;
        }
    }

    return ret;
}

void KDocument::setValue(const QString &path, const QString &value)
{
    QDomNode n = KDocument::buildNode(*this, *this, path);
    if (n.hasChildNodes())
        n.firstChild().setNodeValue(value);
    else
        n.appendChild(createTextNode(value));
}

QString KDocument::buildNodePath(QDomNode &node)
{
    QString path = node.nodeName();
    if (path.indexOf("#") == 0)
    {
        path = "";
    }

    QDomNode parent = node.parentNode();
    if (!parent.isNull())
    {
        QString parentPath = buildNodePath(parent);
        if (!parentPath.isEmpty())
        {
            if (path.isEmpty())
                path = parentPath;
            else
                path = parentPath + QString("/") + path;
        }
    }
    return path;
}

QDomNode KDocument::buildNode(QDomDocument &doc, QDomNode &parent, const QString &path)
{
    QString nodeName = path.section("/", 0, 0);
    QString childPath = path.section("/", 1, 100);

    if (nodeName.isEmpty())
    {
        nodeName = path;
    }

    QDomNode n = parent.firstChildElement(nodeName);
    if (n.isNull() && !nodeName.isEmpty())
    {
        n = parent.appendChild(doc.createElement(nodeName));
    }

    if (!n.isNull() && !childPath.isEmpty())
    {
        n = buildNode(doc, n, childPath);
    }

    return n;
}
