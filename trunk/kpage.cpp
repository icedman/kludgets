#include "config.h"
#include "kludget.h"
#include "kwindow.h"
#include "kview.h"
#include "kpage.h"
#include "knetwork.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QWebPage>

KPage::KPage(QWebView *parent) :
        QWebPage(parent)
{
    setNetworkAccessManager(KNetwork::instance());
}

QString KPage::chooseFile(QWebFrame *originatingFrame, const QString& oldFile)
{
    return QString("");
}

void KPage::javaScriptAlert(QWebFrame *originatingFrame, const QString& msg)
{
    qDebug("alert %s", qPrintable(msg));
    //QWebPage::javaScriptAlert(originatingFrame, msg);
}

bool KPage::javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg)
{
    return true;
}

bool KPage::javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result)
{
    return false;
}

void KPage::javaScriptConsoleMessage(const QString& msg, unsigned int lineNumber, const QString& sourceID)
{
    qDebug("message %d %s", lineNumber, qPrintable(sourceID));
    qDebug(qPrintable(msg));
}

QWebPage *KPage::createWindow(QWebPage::WebWindowType t)
{
    QWebView *w = new QWebView;
    return w->page();
}

bool KPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if (!frame)
    {
        QDesktopServices desktop;
        desktop.openUrl(request.url());
        return false;
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}