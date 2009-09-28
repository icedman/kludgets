#ifndef KLUDGETPAGE_H
#define KLUDGETPAGE_H

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QNetworkRequest>

class KPage : public QWebPage
{
public:
    KPage(QWebView *parent = 0);

    QString chooseFile(QWebFrame *originatingFrame, const QString& oldFile);
    void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
    bool javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg);
    bool javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result);
    void javaScriptConsoleMessage(const QString& msg, unsigned int lineNumber, const QString& sourceID);
	bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);

	QWebPage *createWindow(QWebPage::WebWindowType);
};

#endif
