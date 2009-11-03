#include "config.h"
#include "kludget.h"
#include "kclient.h"
#include "kdocument.h"
#include "ksettings.h"
#include "kwindow.h"
#include "kview.h"
#include "ksystem.h"
#include "knetwork.h"
#include "klog.h"
#include "version.h"

#include "prefwindow.h"
#include "installwindow.h"

#include <QtXml>
#include <QUrl>
#include <QFileDialog>

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
    connect(window->view()->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
    connect(window->view()->page(), SIGNAL(frameCreated(QWebFrame*)), this, SLOT(onFrameCreated(QWebFrame*)));

    connect(this, SIGNAL(evaluate(const QString &)), this, SLOT(onEvaluate(const QString &)));
    connect(&customMenuMapper, SIGNAL(mapped(const QString &)), this, SIGNAL(evaluate(const QString &)));

    KLog::log("Kludget::created");
}

Kludget::~Kludget()
{
    if (prefWindow)
        delete prefWindow;
    KLog::log("Kludget::destroyed");
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

#if 1

    qDebug("path: %s", qPrintable(info.path));
    qDebug("name: %s", qPrintable(info.name));
    qDebug("config: %s", qPrintable(info.configFile));
    qDebug("instance config: %s", qPrintable(info.instancePreferenceFile));
    qDebug("storage: %s", qPrintable(info.storagePath));
    qDebug("contentSrc: %s", qPrintable(info.contentSrc));
#endif

    if (!QFile::exists(info.configFile))
    {
        KLog::log("Kludget::load fail");
        KLog::log("config file not found");
        return false;
    }

    window->setWindowTitle(info.id + ":" + QString::number(QApplication::applicationPid()));

    // access
    KDocument access;
    access.openDocument(info.storagePath + "/access.xml");
    bool accessLocal = access.getValue("kludget/access/local", "0").toInt();
    bool accessNetwork = access.getValue("kludget/access/network", "0").toInt();
    bool accessPlugins = access.getValue("kludget/access/plugins", "0").toInt();
    bool accessSystem = access.getValue("kludget/access/system", "0").toInt();

    // engine
    KDocument engine;
    engine.openDocument(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + ENGINE_CONFIG_FILE);

    // instance settings
    settings->setPath(info.instancePreferenceFile);
#if defined(WIN32)

    settings->loadPreferences(":resources/xml/widgetPreferences.xml");
#else

    settings->loadPreferences(":resources/xml/widgetPreferences_linux.xml");
#endif

    settings->loadPreferences(info.configFile);
    settings->loadPreferences(info.path + "/" + PREFERENCE_FILE);

    // position
    int x = settings->read("kludget/x", window->x()).toInt();
    int y = settings->read("kludget/y", window->y()).toInt();
    window->move(x, y);

    resize(settings->read("kludget/width", info.width).toInt(), settings->read("kludget/height", info.height).toInt());

    window->setOpacity(settings->read("kludget/opacity", 200).toInt());
    window->setIgnoreDrag(settings->read("kludget/ignoreDrag", "0").toInt());
    window->setIgnoreMouse(settings->read("kludget/ignoreMouse", "0").toInt());
    window->setWindowLevel(settings->read("kludget/windowLevel", "0").toInt());
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
    webSettings->setWebGraphic(QWebSettings::MissingImageGraphic, QPixmap());

    // network settings
    KNetwork *net = KNetwork::instance();
    net->loadSettings();
    net->setAccess(accessNetwork, accessLocal, QUrl(QString("file:///") + QFileInfo(info.contentSrc).absolutePath()));

    // system settings
    system->setEnableCommands(accessSystem);
    if (engine.getValue("kludget/general/runInShell", "0").toInt())
        system->setShellPath(engine.getValue("kludget/general/shellPath", ""));

    // plugin
    if (!plugin.isLoaded() && accessPlugins && info.pluginPath != "")
    {
        plugin.setFileName(info.pluginPath + "/" + info.pluginExecutable);
        if (plugin.load())
        {
            typedef void (*initWithWebView)(QWebView*);
            initWithWebView init = (initWithWebView)plugin.resolve("initWithWebView");
            if (init)
            {
                init((QWebView*)window->view());
            }
        }
    }

    // drop
    window->view()->setAcceptDrops(true);

    if (info.debug)
        inspect();

    if (loadPage)
    {
        window->hide();
        QUrl url(QString("file:///%1").arg(info.contentSrc));
        if (!QFile::exists(info.contentSrc))
        {
            url = QUrl(info.contentSrc);
            if (url.scheme().toLower() == "http")
            {
                window->view()->load(url);
            }
            else if (info.contentHtml == "")
            {
                KLog::log("Kludget::load fail");
                KLog::log(QString("content source not found. ") + info.contentSrc);
                return false;
            }
        }

        if (info.contentHtml != "")
            window->view()->setHtml(info.contentHtml);
        else
            window->view()->load(url);

        KLog::log(QString("Kludget::load ") + info.id);

#if 0
        QString defaultBg = info.path + "/Default.png";
        if (QFile::exists(defaultBg))
            //window->view()->setTransitionLayer(QImage(defaultBg));
#endif
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
    frame->addToJavaScriptWindowObject("Kludget", this);
    frame->addToJavaScriptWindowObject("System", system);

    runJavaScriptFile(frame, ":resources/scripts/json2.js");
    runJavaScriptFile(frame, ":resources/scripts/system.js");
    runJavaScriptFile(frame, ":resources/scripts/widget.js");
    runJavaScriptFile(frame, ":resources/scripts/debug.js");

    // add plugin here
    if (plugin.isLoaded())
    {
        typedef void (*windowScriptObjectAvailable)(QWebFrame*);
        windowScriptObjectAvailable wsoAvailable = (windowScriptObjectAvailable)plugin.resolve("windowScriptObjectAvailable");
        if (wsoAvailable)
        {
            // KLog::log("plugin::windowScriptObjectAvailable");
            wsoAvailable(frame);
        }
    }
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

#if defined(WIN32)

    loadMenuFile(":resources/xml/widgetContextMenu.xml");
#else

    loadMenuFile(":resources/xml/widgetContextMenu_linux.xml");
#endif
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
                QString name = menuItem.firstChild().nodeValue();
                QString script = menuItem.attributes().namedItem("action").nodeValue();

                if (menuItem.nodeName().toLower() == "custom_menu")
                {
                    loadMenuFile(info.configFile);
                    loadMenuFile(info.path + "/" + MENU_FILE);
                    continue;
                }

                if (menuItem.nodeName().toLower() == "separator")
                {
                    contextMenu.insertSeparator(0);
                    continue;
                }

                if (script == "")
                    continue;

                QAction *action = contextMenu.addAction(name);
                connect(action, SIGNAL(triggered()), &customMenuMapper, SLOT(map()));
                customMenuMapper.setMapping(action, script);
            }
        }
    }
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
    KLog::instance()->loadSettings();
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
    onEvaluate("getSelection().empty()");
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
    qDebug("onUrlReceived: %s", qPrintable(url->toString()));
    onEvaluate(QString("Kludget.onUrlReceived(") + url->toString() + ")");
}

void Kludget::onSystemExecUpdate(long id)
{
    QString obj = QString("_syscmd_") + QString::number(id);
    onEvaluate(obj + ".update()");
}

void Kludget::onSystemExecFinish(long id)
{
    QString obj = QString("_syscmd_") + QString::number(id);
    onEvaluate(obj + ".onfinish()");
}

void Kludget::screenshot(QString path)
{
    if (path == "")
    {
        path = QFileDialog::getSaveFileName(0,
                                            "Save Image",
                                            QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                            "Image Files (*.png *.jpg *.bmp)");
        if (path == "")
            return;
    }
    window->view()->screenshot(path);

    QDesktopServices::openUrl(QUrl(path));
}

void Kludget::show()
{
    onShow();
    window->show();
}

void Kludget::hide()
{
    onHide();
    window->hide();
}

void Kludget::close()
{
    window->hide();
    window->close();

#if 1
    // forcefully remove preference file
    settings->clear();
#endif

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
#if defined(WIN32)

    prefWindow->buildPreferenceMap(":resources/xml/widgetPreferences.xml");
#else

    prefWindow->buildPreferenceMap(":resources/xml/widgetPreferences_linux.xml");
#endif

    prefWindow->setupUI();
    prefWindow->show();

    connect(prefWindow, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(prefWindow, SIGNAL(destroyed()), this, SLOT(onPreferencesClosed()));
}

void Kludget::createInstance(QString instance)
{
    client->createInstance(instance);
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
    //qDebug("prepareForTransition: %s", qPrintable(transition));

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
    window->view()->beginTransition();
}
