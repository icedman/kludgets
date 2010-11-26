#include "config.h"
#include "knetwork.h"
#include "kdocument.h"
#include "klocalefilelist.h"
#include "klog.h"
#include "kutil.h"

#include <QAuthenticator>
#include <QDesktopServices>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QStringList>

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
#if 1

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
            proxy.setPassword(Util::decrypt(doc.getValue("kludget/network/proxyPassword", "")));
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

QNetworkReply* KNetwork::createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData)
{
    QNetworkRequest req = request;
    QUrl url = req.url();
    //req.setRawHeader("Accept-Encoding","gzip");
    //req.setRawHeader("Accept-Encoding","identity");

#if 0
	KLog::log(QString("request:%1 %2\n%3")
			.arg(url.scheme())
			.arg(url.path())
			.arg(base.path()));
#endif

    if (url.scheme() == "resource")
    {
        QString path = ":resources/" + url.toString(QUrl::RemoveScheme);
        path.replace("//", "");
        KLog::log(QString("resources:") + path);
		return createRequestLocalFile(op, path, outgoingData);
    }

    // hack for AppleClasses
    if (url.path().indexOf("AppleClasses") != -1 || url.path().indexOf("WidgetResources") != -1)
    {
        QString path = url.toLocalFile();
        //todo replace path for AppleClasses
        path.replace("/System/Library/WidgetResources", QApplication::applicationDirPath() + "/widgets/resources");
		return createRequestLocalFile(op, path, outgoingData);
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
		//todo
		//QNetworkReply *reply = createRequestLocalFile(op, "", outgoingData);
        //reply->setError(QNetworkReply::ContentAccessDenied, QString(""));
        //return reply;
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
				return createRequestLocalFile(op, path, outgoingData);
            }
        }
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest(op, request, outgoingData);
    reply->ignoreSslErrors();
    return reply;
}


QNetworkReply* KNetwork::createRequestLocalFile(Operation op, const QString& path, QIODevice* outgoingData)
{
	QNetworkRequest request;
	request.setUrl(QUrl::fromLocalFile(path));
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}