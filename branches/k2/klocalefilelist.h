#ifndef LOCALEFILELIST_H
#define LOCALEFILELIST_H

#include <QString>
#include <QMap>
#include <QLocale>

class LocaleFileList
{
public:

    void initialize(const QString &path, const QString &lang);
    bool localized(const QString &file, QString &localized);
    const QString language();

private:
    void addDirectory(const QString &path);

private:
    QString basePath;
    QString baseLangPath;
    QString lang;
    QMap<QString, QString> map;
    QLocale locale;
};

#endif // LOCALEFILELIST_H
