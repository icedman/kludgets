#include "config.h"
#include "knetwork.h"
#include "kdocument.h"
#include "localefilelist.h"

#include <QAuthenticator>
#include <QDesktopServices>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QStringList>

KNetworkReply::KNetworkReply(const QNetworkRequest& req, const QString& path, QObject* parent) : QNetworkReply(parent)
{
    OpenMode mode = QIODevice::ReadOnly;

    setRequest(req);
    setOperation(QNetworkAccessManager::GetOperation);

    m_buf.setFileName(path);
    if (!m_buf.open(mode))
        qDebug("unable to open %s", qPrintable(path));

    open(mode);
    QTimer::singleShot(0, this, SLOT(go()));
}

qint64 KNetworkReply::readData(char* data, qint64 size)
{
    qint64 r = m_buf.read(data, size);
    if (r <= 0)
    {
        m_buf.close();
        close();
        emit finished();
    }
    else
    {
        emit readyRead();
    }
    return r;
}

qint64 KNetworkReply::bytesAvailable() const
{
    return m_buf.size() + QNetworkReply::bytesAvailable();
}

void KNetworkReply::abort()
{}

void KNetworkReply::go()
{
    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, "OK");
    emit metaDataChanged();
    emit readyRead();
}

KNetwork::KNetwork(QObject *parent) :
        QNetworkAccessManager(parent),
        allowLocalAccess(false),
        allowNetworkAccess(false),
        localefiles(0)
{
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*, QAuthenticator*)));
    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
}

KNetwork* KNetwork::instance()
{
    static KNetwork inst;
    return &inst;
}

void KNetwork::loadSettings()
{
#if 0

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    diskCache->setCacheDirectory(location);
    setCache(diskCache);
#endif

    QNetworkProxy proxy;
    KDocument doc;
    if (doc.openDocument(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE))
    {
        if (doc.getValue("kludget/network/enableProxy", "0").toInt() != 0)
        {
            QString host = doc.getValue("kludget/network/proxyHost", "");
            QString port = "";
            if (host.indexOf(":") != -1)
            {
                QStringList tmp = host.split(":");
                host = tmp.at(0);
                port = tmp.at(1);
            }
            proxy = QNetworkProxy::HttpProxy;
            proxy.setHostName(host);
            proxy.setPort(port.toInt());
            proxy.setUser(doc.getValue("kludget/network/proxyUser", ""));
            proxy.setPassword(doc.getValue("kludget/network/proxyPassword", ""));
        }
    }
    setProxy(proxy);
}

void KNetwork::setAccess(bool network, bool local, const QUrl &b)
{
    allowNetworkAccess = network;
    allowLocalAccess = local;
    base = b;
}

void KNetwork::setLocaleFiles(LocaleFileList *files)
{
    localefiles = files;
}

void KNetwork::authenticationRequired(QNetworkReply *reply, QAuthenticator *auth)
{
    // open authentication dialog
    auth->setUser("");
    auth->setPassword("");
}

void KNetwork::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth)
{
    // open authentication dialog
    auth->setUser("");
    auth->setPassword("");
}

QNetworkReply* KNetwork::createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData)
{
    QUrl url = req.url();

    //qDebug("request:%s %s\n%s", qPrintable(url.scheme()), qPrintable(url.path()), qPrintable(base.path()));

    if (url.scheme() == "resource")
    {
        QString path = ":resources/" + url.toString(QUrl::RemoveScheme);
        path.replace("//", "");
        qDebug("resource: %s", qPrintable(path));
        return new KNetworkReply(req, path, this);
    }

    // hack for AppleClasses
    if (url.path().indexOf("AppleClasses") != -1 || url.path().indexOf("WidgetResources") != -1)
    {
        QString path = url.toLocalFile();

        //todo replace path for AppleClasses
        path.replace("/System/Library/WidgetResources", QApplication::applicationDirPath() + "/widgets/Resources");

        qDebug("appleClasses: %s", qPrintable(path));
        return new KNetworkReply(req, path, this);
    }

    bool accessDenied = false;

    // check access to local files outside of widget
    if (!allowLocalAccess)
    {
        if (url.scheme() == "file" && base.isValid())
        {
            if (!base.isParentOf(url))
            {
                accessDenied = true;
            }
        }
    }

    // check network access
    if (!allowNetworkAccess)
    {
        if (url.scheme() != "file")
        {
            accessDenied = true;
        }
    }

    if (accessDenied)
    {
        KNetworkReply *reply = new KNetworkReply(req, "", this);
        reply->setError(QNetworkReply::ContentAccessDenied, QString(""));
        return reply;
    }

    // check locale
    if (url.scheme() == "file")
    {
        if (localefiles)
        {
            QString path;
            if (localefiles->localized(url.toLocalFile(), path))
            {
                qDebug("localized: %s", qPrintable(path));
                return new KNetworkReply(req, path, this);
            }
        }
    }

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
