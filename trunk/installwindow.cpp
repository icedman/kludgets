#include "installwindow.h"
#include "ksettings.h"
#include "kdocument.h"
#include "util.h"

#include <QWebView>
#include <QWebFrame>
#include <QDesktopWidget>
#include <QApplication>

InstallWindow::InstallWindow(const KludgetInfo& i) :
        PreferenceWindow(0),
        info(i)
{
    settings = new KSettings(this);
    settings->setPath(info.storagePath + "/access.xml");
}

void InstallWindow::createHeader()
{
    QWebView *view = new QWebView;
    view->setFixedHeight(140);
    view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    KDocument about;
    about.openDocument(info.configFile);

    QString iconPath = info.path + "/icon.png";
    if (QFile::exists(iconPath))
        about.setValue("widget/icon", iconPath);

    about.transform(":resources/xsl/aboutWidget.xsl");
    view->setHtml(about.toString(), QUrl(QString("file:///") + QApplication::applicationDirPath() + "/widgets/Resources/"));
    layout->addWidget(view);
}

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
