#ifndef KLUDGETNETWORK_H
#define KLUDGETNETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>

class LocaleFileList;

class KNetwork : public QNetworkAccessManager
{
    Q_OBJECT

public:
    KNetwork(QObject* parent = 0);
    void loadSettings();
    void setAccess(bool network, bool local, const QUrl &base);
    void setLocaleFiles(LocaleFileList *files);
    static KNetwork* instance();

protected:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData = 0);
	QNetworkReply* createRequestLocalFile(Operation op, const QString& path, QIODevice* outgoingData = 0);
private slots:

    void authenticationRequired(QNetworkReply *reply, QAuthenticator *auth);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);

private:
    QUrl base;
    bool allowLocalAccess;
    bool allowNetworkAccess;
    LocaleFileList *localefiles;
};

#endif // KLUDGETNETWORK_H
