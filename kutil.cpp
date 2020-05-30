#include "config.h"
#include "kutil.h"
#include "klog.h"

#include <QDesktopWidget>

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
    QStringList args;
    args.push_back("-o");
    args.push_back(source);
    args.push_back("-d");
    args.push_back(destination);

#if defined(WIN32)

    QFileInfo tool("unzip");
    QString unzip = tool.absoluteFilePath();
#else

    QString unzip = "unzip";
#endif

    Util::execute(unzip, args);

    return true;
}

QString Util::execute(const QString &cmd, const QStringList args)
{
    QProcess proc;
    proc.start(cmd, args);
    proc.waitForFinished();

    QString out = proc.readAllStandardOutput();
    QString err = proc.readAllStandardError();

    return out + err;
}

#define XOR_HASH1 "b94a54d87ab421a1d3d5631d1fc04e6c"
#define XOR_HASH2 "66b5e6e290e308e77517d4a1f9871e57"

QString xorString(const QString &source, const QString &hash)
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
    return xorString(xorString(source, XOR_HASH1), XOR_HASH2);
}

QString Util::decrypt(const QString &source)
{
    return xorString(xorString(source, XOR_HASH2), XOR_HASH1);
}

QImage ImageUtil::blendImages(const QImage &image1, double alpha1, const QImage &image2, double alpha2)
{
    QImage resultImage(image1.size(), QImage::Format_ARGB32_Premultiplied);

    QPainter p;
    p.begin(&resultImage);
    //p.fillRect(QRect(0, 0, image1.size().width(), image1.size().height()), Qt::red);

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
    int x = screenRect.x() + (screenRect.width() / 2 - w->width() / 2) + offsetX;
    int y = screenRect.y() + (screenRect.height() / 2 - w->height() / 2) + offsetY;
    w->move(x, y);
}
// Stack Blur Algorithm by Mario Klingemann <mario@quasimondo.com>
void ImageUtil::blur(QImage &img, int radius)
{
    if (radius < 1)
    {
        return ;
    }

    QRgb *pix = (QRgb*)img.bits();
    int w = img.width();
    int h = img.height();
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;

    int *r = new int[wh];
    int *g = new int[wh];
    int *b = new int[wh];
    int *a = new int[wh];
    int rsum, gsum, bsum, asum, x, y, i, yp, yi, yw;
    QRgb p;
    int *vmin = new int[qMax(w, h)];

    int divsum = (div + 1) >> 1;
    divsum *= divsum;
    int *dv = new int[256 * divsum];
    for (i = 0; i < 256*divsum; ++i)
    {
        dv[i] = (i / divsum);
    }

    yw = yi = 0;

    int **stack = new int * [div];
    for (int i = 0; i < div; ++i)
    {
        stack[i] = new int[4];
    }


    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum, aoutsum;
    int rinsum, ginsum, binsum, ainsum;

    for (y = 0; y < h; ++y)
    {
        rinsum = ginsum = binsum = ainsum
                                   = routsum = goutsum = boutsum = aoutsum
                                                                   = rsum = gsum = bsum = asum = 0;
        for (i = - radius; i <= radius; ++i)
        {
            p = pix[yi + qMin(wm, qMax(i, 0))];
            sir = stack[i + radius];
            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);

            rbs = r1 - abs(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            asum += sir[3] * rbs;

            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }
        }
        stackpointer = radius;

        for (x = 0; x < w; ++x)
        {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            a[yi] = dv[asum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (y == 0)
            {
                vmin[x] = qMin(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];

            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            ++yi;
        }
        yw += w;
    }
    for (x = 0; x < w; ++x)
    {
        rinsum = ginsum = binsum = ainsum
                                   = routsum = goutsum = boutsum = aoutsum
                                                                   = rsum = gsum = bsum = asum = 0;

        yp = - radius * w;

        for (i = -radius; i <= radius; ++i)
        {
            yi = qMax(0, yp) + x;

            sir = stack[i + radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            sir[3] = a[yi];

            rbs = r1 - abs(i);

            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;
            asum += a[yi] * rbs;

            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }

            if (i < hm)
            {
                yp += w;
            }
        }

        yi = x;
        stackpointer = radius;

        for (y = 0; y < h; ++y)
        {
            pix[yi] = qRgba(dv[rsum], dv[gsum], dv[bsum], dv[asum]);

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (x == 0)
            {
                vmin[y] = qMin(y + r1, hm) * w;
            }
            p = x + vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];
            sir[3] = a[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            yi += w;
        }
    }
    delete [] r;
    delete [] g;
    delete [] b;
    delete [] a;
    delete [] vmin;
    delete [] dv;
}
