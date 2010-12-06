#ifndef KDOCUMENT_H
#define KDOCUMENT_H

#include <QDomDocument>

class KDocument : public QDomDocument
{
public:
    bool openDocument(const QString &path);
    bool saveDocument(const QString &path);
    bool transform(const QString &xslPath);
    bool contains(const QString &nodePath);
    QString getValue(const QString &nodePath, const QString &defaultValue);
    void setValue(const QString &nodePath, const QString &value);

    static QString buildNodePath(QDomNode &node);
    static QDomNode buildNode(QDomDocument &doc, QDomNode &parent, const QString &path);

};

#endif // KDOCUMENT_H
