
#include <QObject>

class QWebView;
class QWebFrame;

class MyPlugin : public QObject
{
    Q_OBJECT

public:
    MyPlugin();
    void initWithWebView(QWebView *);
    void windowScriptObjectAvailable(QWebFrame *);

private:
    QString loadScript(const QString &path);

    public
Q_SLOTS:

    void sayHi();
    void sayHello();

private:
    QWebView *view;

};

