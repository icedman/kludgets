#include "prefwindow.h"
#include "ksystem.h"
#include "ksettings.h"
#include "util.h"
#include "hotkey.h"

#include <QDomDocument>

class PrefSeparator : public PreferenceWidget
{
public:
    PrefSeparator(const Preference &pref)
    {
        if (pref.title != "")
        {
            addWidget(new QLabel(pref.title));
        }
        QLabel *separator = new QLabel;
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setLineWidth(0);
        separator->setMidLineWidth(1);
        addWidget(separator, 3);
    }
};

class PrefText : public PreferenceWidget
{
public:

    PrefText(const Preference &pref) : data(new QLineEdit())
    {
        setValue(pref.value);
        addWidget(data);

        if (pref.width > 0)
        {
            data->setMaximumWidth(pref.width);
        }
    }

    QString getValue()
    {
        return data->text();
    }

    void setValue(const QString &q)
    {
        data->setText(q);
    }

protected:

    QLineEdit *data;
};

class QHotKey : public QLineEdit
{
public:
    void keyPressEvent(QKeyEvent *ev)
    {
        key = ev->key();
        modifier = 0;

        if (ev->modifiers() & Qt::ShiftModifier)
            modifier = Qt::ShiftModifier;
        if (ev->modifiers() & Qt::ControlModifier)
            modifier = Qt::ControlModifier;
        if (ev->modifiers() & Qt::AltModifier)
            modifier = Qt::AltModifier;

        ev->ignore();

        if (modifier == 0)
        {
            if (key < Qt::Key_F1 || key > Qt::Key_F35)
                modifier = Qt::ControlModifier;
#if defined(WIN32)
            // F12 is reserved for debugger
            if (key == Qt::Key_F12)
                modifier = Qt::ControlModifier;
#endif

        }

        if (HotKey::keyName((Qt::Key)key) == "")
        {
            key = 0;
            modifier = 0;
        }

        update();
    }

    void keyReleaseEvent(QKeyEvent *ev)
    {
        ev->ignore();
    }

    void update()
    {
        if (key == 0)
        {
            setText("");
            return ;
        }

        QString text = "";
        if (modifier == Qt::ShiftModifier)
            text = "Shift + ";
        if (modifier == Qt::ControlModifier)
            text = "Ctrl + ";
        if (modifier == Qt::AltModifier)
            text = "Alt + ";

        text += HotKey::keyName((Qt::Key)key);
        setText(text);
    }

    int modifier;
    int key;
}
;

class PrefHotKey : public PreferenceWidget
{
public:
    PrefHotKey(const Preference &pref)
            : data(new QHotKey())
    {
        addWidget(data);
        if (pref.width > 0)
        {
            data->setMaximumWidth(pref.width);
        }

        setValue(pref.value);
    }

    QString getValue()
    {
        return QString::number(data->key) + ":" + QString::number(data->modifier);
    }

    void setValue(const QString &q)
    {
        QString t = q + ":0";
        data->key = t.split(":")[0].toInt();
        data->modifier = t.split(":")[1].toInt();
        data->update();
    }

private:

    QHotKey *data;
};

class PrefPassword : public PrefText
{
public:
    PrefPassword(const Preference &pref)
            : PrefText(pref)
    {
        data->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    }
};

class PrefSlider : public PreferenceWidget
{
public:

    PrefSlider(const Preference &pref) : data(new QSlider(Qt::Horizontal))
    {
        int max = pref.attributes.value("max", "100").toInt();
        int min = pref.attributes.value("min", "0").toInt();
        int interval = pref.attributes.value("interval", "10").toInt();

        data->setMaximum(max);
        data->setMinimum(min);
        data->setTickInterval(interval);
        data->setTickPosition(QSlider::TicksBelow);

        setValue(pref.value);
        addWidget(data);
    }

    QString getValue()
    {
        return QString::number(data->value(), 10);
    }

    void setValue(const QString &q)
    {
        data->setValue(q.toInt());
    }

protected:

    QSlider *data;
};

class PrefCheckBox : public PreferenceWidget
{
public:

    PrefCheckBox(const Preference &pref) : data(new QCheckBox(pref.title))
    {
        setValue(pref.value);
        addWidget(data);
    }

    QString getValue()
    {
        if (data->isChecked())
            return "1";
        return "0";
    }

    void setValue(const QString &q)
    {
        data->setChecked(q.toInt());
    }

protected:

    QCheckBox *data;
};

class PrefSelect : public PreferenceWidget
{
public:

    PrefSelect(const Preference &pref) : data(new QComboBox())
    {
        setValue(pref.value);
        addWidget(data);

        if (pref.width > 0)
        {
            data->setMaximumWidth(pref.width);
        }
    }

    QString getValue()
    {
        if (data->count() == 0)
            return "";
        return data->itemData(data->currentIndex()).toString();
    }

    void setValue(const QString &q)
    {
        v = q;
    }

    void addOption(const QString &label, const QString &value)
    {
        data->addItem(label, value);
        if (v == value)
            data->setCurrentIndex(data->count() - 1);
    }

protected:

    QComboBox *data;
    QString v;
};

class PrefTime : public PreferenceWidget
{
public:

    PrefTime(const Preference &pref) : data(new QTimeEdit())
    {
        setValue(pref.value);
        addWidget(data);
    }

    QString getValue()
    {
        return data->time().toString();
    }

    void setValue(const QString &q)
    {
        data->setTime(QTime::fromString(q));
    }

protected:

    QTimeEdit *data;
};

class PrefRadioGroup : public PreferenceWidget
{
public:

    PrefRadioGroup(const Preference &pref) : group(new QButtonGroup()), layout(new QVBoxLayout())
    {
        group->setExclusive(true);
        setValue(pref.value);
        addLayout(layout);
    }

    QString getValue()
    {
        QAbstractButton *b = group->button(group->checkedId());
        if (b)
            return b->property("value").toString();
        return "";
    }

    void setValue(const QString &q)
    {
        v = q;
    }

    void addOption(const QString &label, const QString &value)
    {
        QRadioButton *b = new QRadioButton(label);
        b->setProperty("value", value);
        if (v == value)
            b->setChecked(true);
        group->addButton(b);
        group->setId(b, group->buttons().length());
        layout->addWidget(b);
    }

protected:

    QButtonGroup *group;
    QVBoxLayout *layout;
    QString v;
};

class PrefFont : public PreferenceWidget
{
public:

    PrefFont(const Preference &pref) :
            data(new QFontComboBox()),
            bold(new QCheckBox("Bold")),
            italic(new QCheckBox("Italic")),
            layout(new QHBoxLayout())
    {
        setValue(pref.value);
        addLayout(layout);
        layout->addWidget(data);
        layout->addWidget(bold);
        layout->addWidget(italic);
    }

    QString getValue()
    {
        QString ret = data->currentText();
        if (bold->isChecked())
            ret += ";bold";
        if (italic->isChecked())
            ret += ";italic";
        return ret;
    }

    void setValue(const QString &q)
    {
        QStringList s = q.split(";");
        if (s.length() == 0)
            return ;
        data->setCurrentFont(QFont(s[0]));
        bold->setChecked((q.indexOf("bold") != -1));
        italic->setChecked((q.indexOf("italic") != -1));
    }

protected:

    QHBoxLayout *layout;
    QCheckBox *bold;
    QCheckBox *italic;
    QFontComboBox *data;
};


class PrefColor : public PreferenceWidget
{

class ColorBox : public QWidget
    {
    public:
        void paintEvent(QPaintEvent * /* event */)
        {
            QPainter painter(this);
            QStyleOption option(QStyleOption::SO_Frame);
            option.initFrom(this);
            style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, &painter, this);
        }
    };

public:

    PrefColor(const Preference &pref) :
            box(new ColorBox()),
            btn(new QPushButton("...")),
            layout(new QHBoxLayout())
    {
        setValue(pref.value);
        addLayout(layout);
        layout->addWidget(box);
        layout->addWidget(btn);

        //layout->addWidget(new QWidget());
        btn->setMaximumSize(20, 24);
        box->setMaximumSize(100, 20);
        box->setMinimumSize(100, 20);
        box->setAutoFillBackground(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(selectColor()));
    }

    QString getValue()
    {
        return color().name();
    }

    void setValue(const QString &q)
    {
        setColor(QColor(q));
    }

    void colorSelected(QColor c)
    {
        setColor(c);
    }

    QColor currentColor()
    {
        return color();
    }

protected:

    QColor color()
    {
        return box->palette().color(QPalette::Window);
    }

    void setColor(QColor c)
    {
        QPalette p;
        p.setColor(QPalette::Window, c);
        box->setPalette(p);
    }

protected:

    QHBoxLayout *layout;
    ColorBox *box;
    QPushButton *btn;
};

void PreferenceWidget::selectColor()
{
    QColor c = QColorDialog::getColor(currentColor(), 0);
    if (c.isValid())
    {
        colorSelected(c);
    }
}

PreferenceWidget* PreferenceWidget::create(const Preference &pref)
{
    if (pref.type == "text")
        return new PrefText(pref);

    if (pref.type == "separator")
        return new PrefSeparator(pref);

    if (pref.type == "hotkey")
        return new PrefHotKey(pref);

    if (pref.type == "password")
        return new PrefPassword(pref);

    if (pref.type == "slider")
        return new PrefSlider(pref);

    if (pref.type == "checkbox")
        return new PrefCheckBox(pref);

    if (pref.type == "select")
        return new PrefSelect(pref);

    if (pref.type == "time")
        return new PrefTime(pref);

    if (pref.type == "radio")
        return new PrefRadioGroup(pref);

    if (pref.type == "font")
        return new PrefFont(pref);

    if (pref.type == "color")
        return new PrefColor(pref);

    return 0;
}

PreferenceTab::PreferenceTab()
{
    layout = new QGridLayout;
    layout->setColumnMinimumWidth(0, 0);
    layout->setColumnMinimumWidth(1, 300);
    setLayout(layout);
}

PreferenceWindow::PreferenceWindow(KSettings *s) :
        settings(s),
        currentAction(0),
        defaultCategory("general")
{
    setWindowTitle("Preferences");
}

PreferenceWindow::~PreferenceWindow()
{}

void PreferenceWindow::buildPreferenceMap(const QString &path, bool checkSettings)
{
    QFile file(path);
    QDomDocument dom;
    if (!file.exists())
        return ;

    file.open(QIODevice::ReadOnly);
    dom.setContent(file.readAll());
    file.close();


    QDomNodeList preferences = dom.elementsByTagName("preferences");
    if (!preferences.length())
        return ;

    QDomNodeList prefs = preferences.at(0).childNodes();
    if (!prefs.length())
        return ;

    QString cat = "general";
    for (int i = 0; i < prefs.length(); i++)
    {
        QDomNode n = prefs.at(i);
        Preference pref;

        pref.name = n.attributes().namedItem("name").nodeValue();
        pref.type = n.namedItem("type").firstChild().nodeValue().toLower();
        pref.title = n.namedItem("title").firstChild().nodeValue();
        pref.description = n.namedItem("description").firstChild().nodeValue();
        pref.category = n.namedItem("category").firstChild().nodeValue();
        pref.defaultValue = n.namedItem("default").firstChild().nodeValue();
        pref.width = n.attributes().namedItem("width").nodeValue().toInt();
        pref.height = n.attributes().namedItem("height").nodeValue().toInt();

        if (pref.category.isEmpty())
            pref.category = cat;

        cat = pref.category;

        for (int j = 0; j < n.attributes().length(); j++)
        {
            QDomNode attrib = n.attributes().item(j);
            pref.attributes.insert(attrib.nodeName(), attrib.nodeValue());
        }

        if (pref.type == "password" && !pref.attributes.contains("encrypt"))
        {
            pref.attributes.insert("encrypt", "1");
        }

        if (pref.name != "" && checkSettings)
        {
            if (!settings->contains(pref.name))
                continue;
        }

        if (pref.type == "")
            continue;

        pref.value =
            settings->read(
                pref.name,
                pref.defaultValue
            ).toString();

        if (pref.attributes.value("encrypt") == "1")
            pref.value = Util::encrypt(pref.value);

        pref.widget = 0;

        // options
        QDomNodeList opts = n.toElement().elementsByTagName("option");
        for (int j = 0; j < opts.length(); j++)
        {
            QDomNode n = opts.at(j);
            pref.options.push_back(PreferenceOption(n.attributes().namedItem("value").nodeValue(), n.firstChild().nodeValue()));
        }

        prefList.push_back(pref);
    }
}

void PreferenceWindow::setupUI()
{
    setWindowModality(Qt::WindowModal);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    setFixedWidth(400);

    setWindowIcon(QIcon(":resources/images/system.png"));

    QFile file(":resources/style/application.css");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(QString(file.readAll()));
    file.close();

    layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    createHeader();
    createToolbar();
    createPages();

    // create the toolbar buttons
    QMap<QString, QString> icons;
    icons.insert("general", ":resources/images/preference.png");
    icons.insert("fonts", ":resources/images/font.png");
    icons.insert("colors", ":resources/images/colors.png");
    icons.insert("windows", ":resources/images/windows.png");
    icons.insert("network", ":resources/images/internet.png");
    icons.insert("packages", ":resources/images/package.png");

    QMap<QString, QString> titles;
    titles.insert("general", "General");
    titles.insert("fonts", "Fonts");
    titles.insert("colors", "Colors");
    titles.insert("windows", "Windows");
    titles.insert("network", "Network");
    titles.insert("packages", "Packages");

    QAction *defaultAction = 0;

    PreferenceTabMap::iterator pit = prefTabMap.begin();
    while (pit != prefTabMap.end())
    {
        QString name = (*pit)->objectName();
        QAction *act = new QAction(QIcon(icons.value(name)), titles.value(name), this);
        act->setCheckable(true);
        if (!defaultAction)
            defaultAction = act;
        if (name == defaultCategory)
            defaultAction = act;
        toolbar->addAction(act);
        pit++;
    }

    if (defaultAction)
        onTriggeredAction(defaultAction);

    createDialogControls();

    WindowUtil::center(this, -50, -100);
}

void PreferenceWindow::createToolbar()
{
    toolbar = new QToolBar(tr("Preference"));
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->setIconSize(QSize(48, 48));
    connect(toolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onTriggeredAction(QAction*)));

    layout->addWidget(toolbar);
}

void PreferenceWindow::createPages()
{
    PreferenceList::iterator it = prefList.begin();
    while (it != prefList.end())
    {
        Preference &pref = (*it);
        addPreference(pref);
        it++;
    }
}

void PreferenceWindow::createDialogControls()
{
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *save, *cancel, *apply;

    layout->addLayout(h);
    h->setContentsMargins(16, 16, 16, 8);
    h->addSpacing(200);
    h->addWidget(save = new QPushButton("Save"));
    h->addWidget(cancel = new QPushButton("Cancel"));
    h->addSpacing(10);
    h->addWidget(apply = new QPushButton("Apply"));

    // save
    connect(save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(save, SIGNAL(clicked()), this, SLOT(close()));

    // cancel
    connect(cancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    // apply
    connect(apply, SIGNAL(clicked()), this, SLOT(onSave()));
}

PreferenceTab* PreferenceWindow::addPage(const QString &name)
{
    QString key = name.toLower();
    PreferenceTab *w = prefTabMap.value(key);

    if (!w)
    {
        w = new PreferenceTab();
        w->setObjectName(key);
        layout->addWidget(w);
        prefTabMap.insert(key, w);
    }

    return prefTabMap.value(key);
}

bool PreferenceWindow::addPreference(Preference &pref)
{
    PreferenceTab *tab = addPage(pref.category);
    Q_ASSERT(tab);

    QGridLayout *g = tab->layout;
    int r = g->rowCount();

    if (!pref.title.isEmpty() && pref.type != "checkbox" && pref.type != "separator")
    {
        QLabel *label = new QLabel(pref.title);
        label->setContentsMargins(0, 4, 10, 0);
        g->addWidget(label, r, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    PreferenceWidget *item = PreferenceWidget::create(pref);
    if (item)
    {
        pref.widget = item;
        if (pref.height == 0)
            pref.height = 24;
        item->setSize(pref.width, pref.height);

        // add options
        PreferenceOptionList::iterator it = pref.options.begin();
        while (it != pref.options.end())
        {
            QString v = (*it).value;
            if (v == "")
                v = (*it).text;
            item->addOption((*it).text, v);
            it++;
        }

        if (pref.type == "separator")
            g->addLayout(item, r++, 0, 1, 2);
        else
            g->addLayout(item, r++, 1);
        item->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }

    if (!pref.description.isEmpty())
    {
        QLabel *l = new QLabel(pref.description);
        l->setWordWrap(true);
        g->addWidget(l, r, 1);
    }

    return true;
}

void PreferenceWindow::setDefaultCategory(const QString &cat)
{
    defaultCategory = cat;
}

void PreferenceWindow::onTriggeredAction(QAction *action)
{
    action->setChecked(true);
    if (action == currentAction)
        return ;

    PreferenceTabMap::iterator it = prefTabMap.begin();
    while (it != prefTabMap.end())
    {
        (*it)->hide();
        it++;
    }

    if (currentAction)
        currentAction->setChecked(false);
    currentAction = action;

    PreferenceTab *t = prefTabMap.value(action->text().toLower());
    if (t)
        t->show();

    adjustSize();
}

void PreferenceWindow::onSave()
{
    PreferenceList::iterator it = prefList.begin();
    while (it != prefList.end())
    {
        Preference &pref = (*it);
        if (pref.widget && pref.name != "")
        {
            QString value = pref.widget->getValue();
            if (pref.attributes.value("encrypt") == "1")
                value = Util::encrypt(value);
            settings->write(pref.name, value);
        }
        it++;
    }

    settings->sync();
    emit settingsChanged();
    emit saved();
}

void PreferenceWindow::onCancel()
{
    bool revereted = false;
    PreferenceList::iterator it = prefList.begin();
    while (it != prefList.end())
    {
        Preference &pref = (*it);
        if (pref.widget)
        {
            QString v = pref.widget->getValue();
            if (v != pref.value)
            {
                QString value = pref.value;
                if (pref.attributes.value("encrypt") == "1")
                    value = Util::encrypt(value);
                settings->write(pref.name, value);
                revereted = true;
            }
        }
        it++;
    }

    if (revereted)
    {
        settings->sync();
        emit settingsChanged();
    }

    emit cancelled();
    close();
}
