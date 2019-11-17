#include "installwindow.h"
#include "ksettings.h"
#include "kdocument.h"
#include "kapp.h"
#include "kutil.h"

#include <QtWidgets>
#include <QWebView>
#include <QWebFrame>
#include <QDesktopWidget>
#include <QApplication>

InstallWindow::InstallWindow(const KludgetInfo& i) :
        PreferenceWindow(0),
        info(i),
        view(new QWebView)
{
    settings = new KSettings(this);
    settings->setPath(info.storagePath + "/access.xml");
}

void InstallWindow::createHeader()
{
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    KDocument about;
    about.openDocument(info.configFile);

    QString iconPath = info.path + "/icon.png";
    if (QFile::exists(iconPath))
        about.setValue("widget/icon", iconPath);
    else
        about.setValue("widget/icon", "package.png");

    about.transform(":resources/xsl/aboutWidget.xsl");
	view->setHtml(about.toString(), QUrl::fromLocalFile(QApplication::applicationDirPath() + "/widgets/resources/"));
    layout->addWidget(view);

    /*
    QString url = about.getValue("widget/about/url","");
    if (url !=  "")
    	QDesktopServices::openUrl(QUrl(url));
    */

    QPalette pal = palette();
    pal.setBrush(QPalette::Background, Qt::white);
    setPalette(pal);

    view->setFixedHeight(150);
}

void InstallWindow::onUpdate()
{}

void InstallWindow::createDialogControls()
{
    toolbar->setVisible(false);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *save, *cancel;

    layout->addLayout(h);
    h->setContentsMargins(16, 16, 16, 8);
    h->addSpacing(240);
    h->addWidget(save = new QPushButton("Install"));
    h->addWidget(cancel = new QPushButton("Cancel"));

    // save
    connect(save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(save, SIGNAL(clicked()), this, SLOT(close()));

    // cancel
    connect(cancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

AboutWindow::AboutWindow(const KludgetInfo& info) :
        InstallWindow(info)
{}

void AboutWindow::createDialogControls()
{
    PreferenceTab *w = prefTabMap.value("package");
    if (w)
        w->hide();

    toolbar->setVisible(false);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *security, *ok, *save, *cancel;

    layout->addLayout(h);
    h->setContentsMargins(16, 16, 16, 8);
    h->addWidget(security = new QPushButton("Security"));
    h->addSpacing(300);
    h->addWidget(ok = new QPushButton("Ok"));
    h->addWidget(save = new QPushButton("Save"));
    h->addWidget(cancel = new QPushButton("Cancel"));

    // save
    connect(security, SIGNAL(clicked()), this, SLOT(onSecurity()));
    connect(ok, SIGNAL(clicked()), this, SLOT(close()));
    connect(save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(save, SIGNAL(clicked()), this, SLOT(close()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

    security->setObjectName("security");
    ok->setObjectName("ok");
    save->setObjectName("save");
    save->hide();
    cancel->setObjectName("cancel");
    cancel->hide();

    security->setFixedWidth(100);
    save->setFixedWidth(100);
    ok->setFixedWidth(100);
    cancel->setFixedWidth(100);

    adjustSize();
}

void AboutWindow::onSecurity()
{
    PreferenceTab *w = prefTabMap.value("package");
    if (w)
        w->show();

    QPushButton *btn = 0;

    btn = findChild<QPushButton*>("security");
    if (btn)
        btn->hide();

    btn = findChild<QPushButton*>("ok");
    if (btn)
        btn->hide();

    btn = findChild<QPushButton*>("save");
    if (btn)
        btn->show();

    btn = findChild<QPushButton*>("cancel");
    if (btn)
        btn->show();

    adjustSize();
}

AboutKludgetWindow::AboutKludgetWindow()
        : PreferenceWindow(0),
        view(new QWebView)
{}

void AboutKludgetWindow::createHeader()
{
    //setWindowIcon(QIcon(":resources/images/kludget.png"));

    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    KDocument about;
    about.setValue("widget/name", "Kludget Engine");
    about.setValue("widget/version", KApp::version());
    about.setValue("widget/userAgent", KApp::userAgent());
    about.setValue("widget/logo", "kludget.png");
    about.setValue("widget/icon", "engine.png");

    about.transform(":resources/xsl/aboutKludget.xsl");
    view->setHtml(about.toString(), QUrl::fromLocalFile(QApplication::applicationDirPath() + "/widgets/resources/"));
    layout->addWidget(view);

    QPalette pal = palette();
    pal.setBrush(QPalette::Background, Qt::white);
    setPalette(pal);

    view->setFixedHeight(240);
}

void AboutKludgetWindow::createDialogControls()
{
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *credits, *site, *close;

    layout->addLayout(h);
    h->setContentsMargins(16, 16, 16, 8);
    h->addWidget(site = new QPushButton("Visit Website"));
    h->addWidget(credits = new QPushButton("Credits"));
    h->addSpacing(300);
    h->addWidget(close = new QPushButton("Ok"));

    // save
    connect(site, SIGNAL(clicked()), this, SLOT(onWebSite()));
    connect(credits, SIGNAL(clicked()), this, SLOT(onCredits()));
    connect(close, SIGNAL(clicked()), this, SLOT(close()));
}

void AboutKludgetWindow::onWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.kludgets.com/download"));
}

void AboutKludgetWindow::onCredits()
{
    QDesktopServices::openUrl(QUrl("http://code.google.com/p/kludgets/wiki/Credits"));
}
