#include "localefilelist.h"

#include <QDir>
#include <QLocale>

void LocaleFileList::initialize(const QString &path, const QString &l)
{
    basePath = QDir(path).absolutePath();
    lang = l;

    QLocale locale(l.toLower());
    QDir dir(path);

    QStringList dirList;
    dirList.push_back(QDir(path).absolutePath() + "/" + locale.name().split("_")[0] + ".lproj");
    dirList.push_back(QDir(path).absolutePath() + "/" + locale.languageToString(locale.language()) + ".lproj");

    QStringList::iterator it = dirList.begin();
    while (it != dirList.end())
    {
        if (QDir(*it).exists())
        {
            baseLangPath = (*it);
            addDirectory(baseLangPath);
        }
        it++;
    }
}

const QString LocaleFileList::language()
{
    return lang;
}

bool LocaleFileList::localized(const QString &file, QString &localized)
{
    //qDebug("check: %s", qPrintable(file));
    if (!map.contains(file))
        return false;
    localized = map.value(file);
    return true;
}

void LocaleFileList::addDirectory(const QString &path)
{
    //qDebug("add locale dir: %s", qPrintable(path));
    QDir directory(path);
    QStringList dirs = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList::iterator dit = dirs.begin();
    while (dit != dirs.end())
    {
        addDirectory(directory.absolutePath() + "/" + (*dit));
        dit++;
    }

    QStringList files = directory.entryList(QDir::Files);
    QStringList::iterator fit = files.begin();
    while (fit != files.end())
    {
        QString fullPath = directory.absolutePath() + "/" + (*fit);
        QString pathName = fullPath;
        pathName.replace(baseLangPath + "/", "");
        map.insert(basePath + "/" + pathName, fullPath);
        //qDebug("add locale file:\n%s\n%s", qPrintable(pathName), qPrintable(fullPath));
        fit++;
    }
}
