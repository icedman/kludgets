#include "config.h"
#include "util.h"

bool Util::copyDir(const QString &source, const QString &destination, const bool overwrite)
{
    QDir directory(source);
    bool error = false;

    if (!directory.exists())
    {
        return false;
    }

    QDir destDir(destination);
    destDir.mkpath(destination);

    QStringList dirs = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList files = directory.entryList(QDir::Files);

    QList<QString>::iterator d, f;

    for (d = dirs.begin(); d != dirs.end(); ++d)
    {
        if (!QFileInfo(directory.path() + "/" + (*d)).isDir())
        {
            continue;
        }

        QDir temp(destination + "/" + (*d));
        temp.mkpath(temp.path());

        if (!copyDir(directory.path() + "/" + (*d), destination + "/" + (*d), overwrite))
        {
            error = true;
        }
    }

    for (f = files.begin(); f != files.end(); ++f)
    {

        QFile tempFile(directory.path() + "/" + (*f));

        if (QFileInfo(directory.path() + "/" + (*f)).isDir())
        {
            continue;
        }

        QFile destFile(destination + "/" + directory.relativeFilePath(tempFile.fileName()));

        if (destFile.exists() && overwrite)
        {
            destFile.remove();
        }

        if (!tempFile.copy(destination + "/" + directory.relativeFilePath(tempFile.fileName())))
        {
            error = true;
        }
    }

    return !error;
}

bool Util::deleteDir(const QString &source)
{
    QDir directory(source);
    bool error = false;

    if (!directory.exists())
    {
        return false;
    }

    QStringList dirs = directory.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QStringList files = directory.entryList(QDir::Files);

    QList<QString>::iterator d, f;

    for (d = dirs.begin(); d != dirs.end(); ++d)
    {
        QString dirPath = directory.path() + "/" + (*d);
        if (!QFileInfo(dirPath).isDir())
        {
            continue;
        }
        if (!deleteDir(dirPath))
        {
            error = true;
        }
        directory.rmdir(dirPath);
    }

    for (f = files.begin(); f != files.end(); ++f)
    {
        QFile::remove
            (directory.path() + "/" + (*f));
    }

    QString dir = directory.dirName();
    if (directory.cdUp())
    {
        directory.rmdir(dir);
    }

    return !error;
}

bool Util::extract(const QString &source, const QString &destination)
{
    // todo..

    QStringList args;
    args.push_back("-o");
    args.push_back(source);
    args.push_back("-d");
    args.push_back(destination);

    QFileInfo tool("unzip");
    QString unzip = tool.absoluteFilePath();

    int ret = QProcess::execute(unzip, args);

    return true;
}


#define XOR_HASH1 "b94a54d87ab421a1d3d5631d1fc04e6c"
#define XOR_HASH2 "66b5e6e290e308e77517d4a1f9871e57"

QString xor(const QString &source, const QString &hash)
{
    QString res;
    const ushort *cs = source.utf16();
    const ushort *ch = hash.utf16();
    for (int i = 0; i < source.length(); i++)
    {
        int j = i % 32;
        res.append(cs[i] ^ ch[j]);
    }
    return res;
}

QString Util::encrypt(const QString &source)
{
    return xor(xor(source, XOR_HASH1), XOR_HASH2);
}

QString Util::decrypt(const QString &source)
{
    return xor(xor(source, XOR_HASH2), XOR_HASH1);
}

QImage ImageUtil::blendImages(const QImage &image1, double alpha1, const QImage &image2, double alpha2)
{
    QImage resultImage(image1.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter p;
    p.begin(&resultImage);
    //p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    if (alpha2 > 0)
    {
        p.setOpacity(alpha2);
        p.drawImage(0, 0, image2);
    }
    if (alpha1 > 0)
    {
        p.setOpacity(alpha1);
        p.drawImage(0, 0, image1);
    }
    p.end();

    if (alpha1 > 0.8)
        resultImage.setAlphaChannel(image1.alphaChannel());

    return resultImage;
}

QImage ImageUtil::grayedImage(const QImage &image)
{
    QImage img = image;
    int pixels = img.width() * img.height();
    unsigned int *data = (unsigned int *)img.bits();
    for (int i = 0; i < pixels; ++i)
    {
        int val = qGray(data[i]);
        data[i] = qRgba(val, val, val, qAlpha(data[i]));
    }
    return img;
}

QImage ImageUtil::tintedImage(const QImage &image, const QColor &color, QPainter::CompositionMode mode)
{
    QImage resultImage(image.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, grayedImage(image));
    painter.setCompositionMode(mode);
    painter.fillRect(resultImage.rect(), color);
    painter.end();
    resultImage.setAlphaChannel(image.alphaChannel());
    return resultImage;
}

void WindowUtil::center(QWidget *w, int offsetX, int offsetY)
{
    QDesktopWidget desktop;
    QRect screenRect = desktop.screenGeometry(w);
    int x = screenRect.x() + (screenRect.width() / 2 - w->width() / 2);
    int y = screenRect.y() + (screenRect.height() / 2 - w->height() / 2);
    w->move(x + offsetX, y + offsetY);
}
