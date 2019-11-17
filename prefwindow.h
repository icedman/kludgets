#ifndef PREFWINDOW_H
#define PREFWINDOW_H

#include "config.h"

#include <QtCore>
#include <QtWidgets>

class KSettings;
class PreferenceWidget;

class PreferenceOption
{
public:
    PreferenceOption(const QString &v, const QString &t) : value(v), text(t) {}
    QString value;
    QString text;
};

typedef QList<PreferenceOption> PreferenceOptionList;

class Preference
{
public:
    Preference() : widget(0) {}
    QString name;
    QString title;
    QString description;
    QString type;
    QString category;
    QString value;
    QString defaultValue;
    PreferenceWidget *widget;
    PreferenceOptionList options;

    QMap<QString, QString> attributes;

    int width;
    int height;
};

typedef QList<Preference> PreferenceList;

class PreferenceWidget : public QHBoxLayout
{
    Q_OBJECT
public:
    virtual QString getValue() {
        return "";
    }
    virtual void setValue(const QString &q) {}
    virtual void addOption(const QString &label, const QString &value) {}
    virtual void setSize(int w, int h) {}
    virtual void setColor(QColor c) {}
    virtual QColor currentColor() {
        return QColor();
    }
    static PreferenceWidget* create(const Preference &pref);

    public
Q_SLOTS:
    void selectColor();
};

class PreferenceTab : public QWidget
{
    Q_OBJECT;
public:
    PreferenceTab();
    QGridLayout *layout;
};

typedef QMap<QString, PreferenceTab*> PreferenceTabMap;

class PreferenceWindow : public QWidget
{
    Q_OBJECT

public:
    PreferenceWindow(KSettings *s = 0);
    ~PreferenceWindow();

    void buildPreferenceMap(const QString &path, bool checkSettings = false);
    void setupUI();

public:
Q_SIGNALS:
    void settingsChanged();
    void saved();
    void cancelled();

protected:

    virtual void createHeader() {}
    virtual void createToolbar();
    virtual void createPages();
    virtual void createDialogControls();

    PreferenceTab* addPage(const QString &name);
    bool addPreference(Preference &pref);
    void setDefaultCategory(const QString &cat);

protected:

    QVBoxLayout *layout;
    QToolBar *toolbar;
    QAction *currentAction;
    QString defaultCategory;

    KSettings *settings;

    private
Q_SLOTS:
    void onTriggeredAction(QAction*);
    void onSave();
    void onCancel();

protected:
    PreferenceTabMap prefTabMap;
    PreferenceList prefList;

};

#endif // PREFWINDOW_H
