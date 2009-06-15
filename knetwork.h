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
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData = 0 );

private slots:
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *auth);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);

private:
    QUrl base;
    bool allowLocalAccess;
    bool allowNetworkAccess;
    LocaleFileList *localefiles;
};

class KNetworkReply : public QNetworkReply
{
    Q_OBJECT
public:
    KNetworkReply(const QNetworkRequest& req, const QString& path, QObject* parent);
    qint64 bytesAvailable() const;
    qint64 readData(char* data, qint64 size);
    void abort();

private slots:
    void go();

private:
    QFile m_buf;

    friend class KNetwork;
};


#endif // KLUDGETNETWORK_H
