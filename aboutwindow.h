#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtCore>
#include <QtGui>

class Kludget;

class KludgetAbout
{
public:
    QString name;
    QString title;
    QString description;
    QString version;
    QString author;
    QString url;
    QString email;
    QString copyright;
};

class AboutWindow : public QWidget
{
    Q_OBJECT

public:
    AboutWindow(Kludget *k);
    ~AboutWindow();

    void buildAbout(QString path);
    void setupUI();

    Kludget *kludget;
    KludgetAbout about;

    QDialogButtonBox *buttonBox;
    QLabel *name;
    QLabel *author;
    QLabel *url;
    QLabel *description;

private slots:
    void onOk();
};

#endif
