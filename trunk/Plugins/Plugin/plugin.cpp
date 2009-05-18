#include "plugin.h"

#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QFile>
#include <QTextStream>

#ifdef Q_WS_WIN
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

MyPlugin plugin;

extern "C" DLL_EXPORT void initWithWebView(QWebView *view)
{
    plugin.initWithWebView(view);
}

extern "C" DLL_EXPORT void windowScriptObjectAvailable(QWebFrame *frame)
{
    plugin.windowScriptObjectAvailable(frame);
}

MyPlugin::MyPlugin()
        : view(0)
{}

void MyPlugin::initWithWebView(QWebView *v)
{
    view = v;
}

void MyPlugin::windowScriptObjectAvailable(QWebFrame *frame)
{
    frame->addToJavaScriptWindowObject("MyPlugin", this);
    frame->evaluateJavaScript(loadScript(":plugin.js"));
}

QString MyPlugin::loadScript(const QString &path)
{
    QFile scriptFile(path);
    if (scriptFile.open(QIODevice::ReadOnly))
    {
        return QTextStream(&scriptFile).readAll();
    }
    return "";
}

void MyPlugin::sayHi()
{
    if (view)
        view->page()->mainFrame()->evaluateJavaScript("alert('hi')");
}

void MyPlugin::sayHello()
{
    if (view)
        view->page()->mainFrame()->evaluateJavaScript("alert('hello')");
}
