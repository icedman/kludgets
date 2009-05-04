#include "config.h"
#include "kludget.h"
#include "kclient.h"
#include "kdocument.h"
#include "ksettings.h"
#include "kwindow.h"
#include "kview.h"
#include "ksystem.h"
#include "knetwork.h"
#include "prefwindow.h"
#include "installwindow.h"

#include <QtXml>
#include <QUrl>

Kludget::Kludget(KClient *parent) :
        QObject(parent),
        client(parent),
        window(new KWindow),
        settings(new KSettings(this)),
        system(new KSystem(this)),
        prefWindow(0),
        aboutWindow(0),
        firstShow(true)
{
    setObjectName("Kludget");
    settings->setRootKey("kludget");

    connect(system, SIGNAL(execUpdate(long)), this, SLOT(onSystemExecUpdate(long)));
    connect(system, SIGNAL(execFinish(long)), this, SLOT(onSystemExecFinish(long)));

    connect(window, SIGNAL(destroyed()), this, SLOT(onWindowDestroyed()));
    connect(window, SIGNAL(onShow()), this, SLOT(onShow()));
    connect(window, SIGNAL(onHide()), this, SLOT(onHide()));
    connect(window, SIGNAL(onStartDrag()), this, SLOT(onStartDrag()));
    connect(window, SIGNAL(onEndDrag()), this, SLOT(onEndDrag()));
    connect(window, SIGNAL(onSettingsChanged()), this, SLOT(onSettingsChanged()));

    connect(window->view(), SIGNAL(contextMenuRequested()), this, SLOT(onContextMenu()));
    connect(window->view(), SIGNAL(urlReceived(const QUrl*)), this, SLOT(onUrlReceived(const QUrl*)));
    connect(window->view()->page(), SIGNAL(loadFinished(bool)), this, SLOT(show()));
    //connect(window->view()->page()->mainFrame(), SIGNAL(initialLayoutCompleted ()), this, SLOT(show()));
    connect(window->view()->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
    connect(window->view()->page(), SIGNAL(frameCreated(QWebFrame*)), this, SLOT(onFrameCreated(QWebFrame*)));

    connect(this, SIGNAL(evaluate(const QString &)), this, SLOT(onEvaluate(const QString &)));
    connect(&customMenuMapper, SIGNAL(mapped(const QString &)), this, SIGNAL(evaluate(const QString &)));
}

Kludget::~Kludget()
{
    if (prefWindow)
        delete prefWindow;
}

Kludget* Kludget::create(KClient *client, const KludgetInfo &i)
{
    Kludget *k = new Kludget(client);

    if (!k->loadSettings(i, true))
    {
        delete k;
        k = 0;
    }

    return k;
}

bool Kludget::loadSettings(const KludgetInfo &i, bool loadPage)
{
    info = i;

    if (!QFile::exists(info.configFile))
        return false;

    // access
    KDocument access;
    access.openDocument(info.storagePath + "/access.xml");
    bool accessLocal = access.getValue("kludget/access/local", "0").toInt();
    bool accessNetwork = access.getValue("kludget/access/network", "0").toInt();
    bool accessPlugins = access.getValue("kludget/access/plugins", "0").toInt();
    bool accessSystem = access.getValue("kludget/access/system", "0").toInt();

    KDocument engine;
    engine.openDocument(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);

    // instance settings
    settings->setPath(info.instancePreferenceFile);

    loadPreferences(":resources/xml/widgetPreferences.xml");
    loadPreferences(info.configFile);
    loadPreferences(info.path + "/" + PREFERENCE_FILE);

    // position
    int x = settings->read("kludget/x", window->x()).toInt();
    int y = settings->read("kludget/y", window->y()).toInt();
    window->move(x, y);

    resize(settings->read("kludget/width", info.width).toInt(), settings->read("kludget/height", info.height).toInt());

    window->setOpacity(settings->read("kludget/opacity", 200).toInt());
    window->setWindowLevel(settings->read("kludget/windowLevel", "0").toInt());
    window->setIgnoreDrag(settings->read("kludget/ignoreDrag", "0").toInt());
    window->setSnapToScreen(settings->read("kludget/snapToScreen", "0").toInt());
    window->view()->setGrayed(settings->read("kludget/grayScaled", "0").toInt());
#if 0

    window->view()->setTinted(settings->read("kludget/tinted", "0").toInt());
    window->view()->setTintColor(QColor(settings->read("kludget/tintColor", "#c0c0c0").toString()));
    window->view()->setTintMode(settings->read("kludget/tintMode", "14").toInt());
#endif

    // zoom
    window->setZoomFactor(settings->read("kludget/zoom", 1).toDouble());
    window->view()->page()->setViewportSize(window->view()->page()->viewportSize());
    window->autoSize(true);

    setProperty("identifier", info.id);
    setProperty("instance", info.instance);

    setupContextMenu();

    QWebSettings *webSettings = window->view()->page()->settings();
    webSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    webSettings->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, true);
    webSettings->setOfflineStoragePath(info.storagePath);
    webSettings->setOfflineStorageDefaultQuota(5000000);
    webSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, accessPlugins);
    webSettings->setUserStyleSheetUrl(QUrl("resource:style/widget.css"));

    // network settings
    KNetwork *net = KNetwork::instance();
    net->loadSettings();
    net->setAccess(accessNetwork, accessLocal, QUrl(QString("file:///") + QFileInfo(info.contentSrc).absolutePath()));

    // system settings
    system->setEnableCommands(accessSystem);
    if (engine.getValue("kludget/general/runInShell", "0").toInt())
    {
        system->setShellPath(engine.getValue("kludget/general/shellPath", ""));
    }

    // drop
    window->view()->setAcceptDrops(true);

    if (info.debug)
        inspect();

    if (loadPage)
    {
        window->hide();
        if (!QFile::exists(info.contentSrc))
        {
            return false;
        }
        window->view()->load(QUrl(QString("file:///") + info.contentSrc));
    }

#if 1

    qDebug("path: %s", qPrintable(info.path));
    qDebug("name: %s", qPrintable(info.name));
    qDebug("config: %s", qPrintable(info.configFile));
    qDebug("instance config: %s", qPrintable(info.instancePreferenceFile));
    qDebug("storage: %s", qPrintable(info.storagePath));
    qDebug("contentSrc: %s", qPrintable(info.contentSrc));
    qDebug("kludget created %s!" , qPrintable(info.instance));
#endif

    return true;
}

bool Kludget::loadPreferences(const QString &path)
{
    QFile file(path);
    QDomDocument dom;
    if (!file.exists())
        return false;

    file.open(QIODevice::ReadOnly);
    dom.setContent(file.readAll());
    file.close();

    QDomNodeList prefs = dom.elementsByTagName("preference");
    if (!prefs.length())
        return false;

    for (int i = 0; i < prefs.length(); i++)
    {
        QDomNode n = prefs.at(i);
        QString name = n.attributes().namedItem("name").nodeValue();
        QString defaultValue = n.namedItem("default").firstChild().nodeValue();
        if (!settings->contains(name))
        {
            settings->write(name, defaultValue);
        }
    }

    return true;
}

void Kludget::saveSettings()
{
    // position
    settings->write("kludget/x", window->x());
    settings->write("kludget/y", window->y());
}

void Kludget::addJavaScriptWindowObjects(QWebFrame* frame)
{
    // add plugin here

    frame->addToJavaScriptWindowObject("Kludget", this);
    frame->addToJavaScriptWindowObject("System", system);

    runJavaScriptFile(frame, ":resources/scripts/json2.js");
    runJavaScriptFile(frame, ":resources/scripts/system.js");
    runJavaScriptFile(frame, ":resources/scripts/widget.js");
}

void Kludget::runJavaScriptFile(QWebFrame* frame, const QString &p)
{
    QFile scriptFile(p);
    if (scriptFile.open(QIODevice::ReadOnly))
    {
        QString script = QTextStream(&scriptFile).readAll();
        frame->evaluateJavaScript(script);
    }
}

void Kludget::setupContextMenu()
{
    contextMenu.clear();

    loadMenuFile(info.configFile);
    loadMenuFile(info.path + "/" + MENU_FILE);
    loadMenuFile(":resources/xml/widgetContextMenu.xml");
}

void Kludget::loadMenuFile(const QString &path)
{
    QFile fmenu(path);
    if (fmenu.exists())
    {
        fmenu.open(QIODevice::ReadOnly);
        QString content = fmenu.readAll();
        fmenu.close();

        QDomDocument dom;
        dom.setContent(content);

        QDomNodeList menuList = dom.elementsByTagName("menu");
        if (menuList.length() > 0)
        {
            QDomNodeList menuItemList = menuList.item(0).childNodes();
            for (int i = 0; i < menuItemList.length(); i++)
            {
                QDomElement menuItem = menuItemList.item(i).toElement();
                if (menuItem.nodeName().toLower() == "separator")
                {
                    contextMenu.insertSeparator(0);
                    continue;
                }

                QString name = menuItem.firstChild().nodeValue();
                QString script = menuItem.attributes().namedItem("action").nodeValue();

                QAction *action = contextMenu.addAction(name);
                connect(action, SIGNAL(triggered()), &customMenuMapper, SLOT(map()));
                customMenuMapper.setMapping(action, script);
            }
        }
    }
}

void Kludget::onBeginShowCountDown()
{
    // hack. show two seconds after
    QTimer::singleShot(2000, this, SLOT(show()));
}

void Kludget::onShow()
{
    qDebug("onShow");
    onEvaluate("Kludget.onShow()");
}

void Kludget::onHide()
{
    qDebug("onHide");
    onEvaluate("Kludget.onHide()");
}

void Kludget::onStartDrag()
{
    qDebug("onStartDrag");
    onEvaluate("Kludget.onStartDrag()");
}

void Kludget::onEndDrag()
{
    qDebug("onEndDrag");
    saveSettings();
    onEvaluate("Kludget.onEndDrag()");
}

void Kludget::onRemove()
{
    qDebug("onRemove");
    onEvaluate("Kludget.onRemove()");
}

void Kludget::onSettingsChanged()
{
    loadSettings(info);
    onEvaluate("Kludget.onSettingsChanged()");
}

void Kludget::onEvaluate(const QString &command)
{
    QString script = QString("try { ") + command + "; } catch(e) { try { " + command.toLower() + "; } catch(e) { alert('" + command + "' + e) } }";
    window->view()->page()->mainFrame()->evaluateJavaScript(script);
}

void Kludget::onWindowDestroyed()
{
    deleteLater();
}

void Kludget::onPreferencesClosed()
{
    prefWindow = 0;
    aboutWindow = 0;
}

void Kludget::onContextMenu()
{
    contextMenu.popup(QCursor::pos());
}

void Kludget::onJavaScriptWindowObjectCleared()
{
    addJavaScriptWindowObjects(window->view()->page()->mainFrame());
}

void Kludget::onFrameCreated(QWebFrame *frame)
{
    addJavaScriptWindowObjects(frame);
    // todo attach javaScriptWindowObjectCleared signal to this frame
}

void Kludget::onUrlReceived(const QUrl *url)
{
    // qDebug("onUrlReceived: %s", qPrintable(url->toString()));
    onEvaluate(QString("Kludget.onUrlReceived(") + url->toString() + ")");
}

void Kludget::onSystemExecUpdate(long id)
{
    QString obj = QString("_syscmd_") + QString::number(id);
    //qDebug("onSystemExecUpdate: %s", qPrintable(obj));
    onEvaluate(obj + ".update()");
}

void Kludget::onSystemExecFinish(long id)
{
    QString obj = QString("_syscmd_") + QString::number(id);
    //qDebug("onSystemExecFinish: %s", qPrintable(obj));
    onEvaluate(obj + ".onfinish()");
}

void Kludget::show()
{
    if (firstShow)
    {
        window->view()->setTransition(KView::FirstShow);
        window->view()->setFrozen(true);
        performTransition();
        firstShow = false;
    }

    window->raise();
    window->show();
}

void Kludget::hide()
{
    window->hide();
}

void Kludget::close()
{
    window->close();

    QFile::remove
        (info.instancePreferenceFile);

    onRemove();
}

void Kludget::inspect()
{
    window->view()->page()->triggerAction(QWebPage::InspectElement);
}

void Kludget::reload()
{
    window->view()->page()->triggerAction(QWebPage::Reload);
}

void Kludget::about()
{
    if (aboutWindow)
    {
        aboutWindow->raise();
        aboutWindow->show();
        return ;
    }

    aboutWindow = new AboutWindow(info);
    aboutWindow->setAttribute(Qt::WA_DeleteOnClose);
    aboutWindow->setWindowTitle("About - " + info.name);
    aboutWindow->buildPreferenceMap(":resources/xml/accessPreferences.xml", false);
    aboutWindow->setupUI();
    aboutWindow->show();

    connect(aboutWindow, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(aboutWindow, SIGNAL(destroyed()), this, SLOT(onPreferencesClosed()));
}

void Kludget::configure(QString cat)
{
    saveSettings();

    if (prefWindow)
    {
        prefWindow->raise();
        prefWindow->show();
        return ;
    }

    prefWindow = new PreferenceWindow(settings);
    prefWindow->setAttribute(Qt::WA_DeleteOnClose);
    prefWindow->setWindowTitle("Preferences - " + info.name);
    prefWindow->buildPreferenceMap(info.configFile);
    prefWindow->buildPreferenceMap(info.path + "/" + PREFERENCE_FILE);
    prefWindow->buildPreferenceMap(":resources/xml/widgetPreferences.xml");
    prefWindow->setupUI();
    prefWindow->show();

    connect(prefWindow, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(prefWindow, SIGNAL(destroyed()), this, SLOT(onPreferencesClosed()));
}

void Kludget::createInstance()
{
    client->createInstance();
}

void Kludget::move(int x, int y)
{
    window->move(x, y);
}

void Kludget::resize(int w, int h)
{
    if (w > 0 && h > 0)
        window->setMinimumSize(QSize(w, h));
    window->resize(w, h);
}

int Kludget::opacity()
{
    return window->opacity();
}

int Kludget::windowLevel()
{
    return window->windowLevel();
}

int Kludget::x()
{
    return window->x();
}

int Kludget::y()
{
    return window->y();
}

int Kludget::width()
{
    return window->width();
}

int Kludget::height()
{
    return window->height();
}

void Kludget::renderLayer(QString layer)
{
    int z = -1;

    if (layer.indexOf("back") != -1)
        z = KView::Background;
    else if (layer.indexOf("fore") != -1)
        z = KView::Foreground;

    if (z != -1)
        window->view()->renderLayer(z);
}

void Kludget::prepareForTransition(QString transition)
{
    qDebug("prepareForTransition: %s", qPrintable(transition));

    int t = KView::Transition;
    if (transition.indexOf("Back") != -1)
        t = KView::ToBack;
    else if (transition.indexOf("Front") != -1)
        t = KView::ToFront;

    window->view()->setTransition(t);
    window->view()->renderLayer(KView::Transition);
    window->view()->setFrozen(true);
}

void Kludget::performTransition()
{
    qDebug("perfromTransition");
    window->view()->beginTransition();
}