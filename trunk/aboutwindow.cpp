#include "aboutwindow.h"
#include "kludget.h"

#include <QDomDocument>

AboutWindow::AboutWindow(Kludget *k) :
        kludget(k)
{
    buildAbout(kludget->info.configFile);
    setupUI();
}

AboutWindow::~AboutWindow()
{}

void AboutWindow::buildAbout(QString path)
{
    QFile file(path);
    QDomDocument dom;

    if (!file.exists())
    {
        qDebug() << "config file not found";
    }
    else
    {
        file.open(QIODevice::ReadOnly);
        dom.setContent(file.readAll());
        file.close();
    }

    QDomElement d = dom.documentElement();

    about.name = d.namedItem("name").firstChild().nodeValue();
    about.title = d.namedItem("title").firstChild().nodeValue();
    about.description = d.namedItem("description").firstChild().nodeValue();
    about.version = d.namedItem("version").firstChild().nodeValue();

    about.author = d.namedItem("author").firstChild().nodeValue();
    QDomNode info = d.namedItem("author").namedItem("info");
    about.url = info.attributes().namedItem("url").nodeValue();
    about.email = info.namedItem("email").nodeValue();

    about.copyright = d.namedItem("copyright").firstChild().nodeValue();

	if (about.name == "")
		about.name = kludget->info.name;
}

void AboutWindow::setupUI()
{
	int w = 300, h = 200;

	setFixedWidth(w);
    setFixedHeight(h);
	
    QDesktopWidget desktop;
    QRect screenRect = desktop.screenGeometry((QWidget*)kludget->window);
    int x = (screenRect.width() / 2) - (w / 2);
    int y = (screenRect.height() / 2) - (h / 2);
    move(screenRect.x() + x, screenRect.y() + y);

    setWindowModality(Qt::WindowModal);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("About - " + about.name + " " + about.version);
    setWindowIcon(QIcon(":resources/images/kludget.png"));

    name = new QLabel(this);
    name->setObjectName(QString::fromUtf8("name"));
    name->setGeometry(QRect(20, 10, 361, 16));
    name->setText(about.name);

    author = new QLabel(this);
    author->setObjectName(QString::fromUtf8("author"));
    author->setGeometry(QRect(20, 30, 361, 16));
    author->setText(about.author);

    url = new QLabel(this);
    url->setObjectName(QString::fromUtf8("url"));
    url->setGeometry(QRect(20, 50, 361, 16));
    url->setText(about.url);

    description = new QLabel(this);
    description->setObjectName(QString::fromUtf8("description"));
    description->setGeometry(QRect(20, 80, w - 20, 141));
    description->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    description->setText(about.description);
	description->setWordWrap(true);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    buttonBox->setGeometry(QRect(30, h - 40, w - 40, 32));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
}

void AboutWindow::onOk()
{
	close();
}