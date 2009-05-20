#ifndef INSTALLWINDOW_H
#define INSTALLWINDOW_H

#include "prefwindow.h"
#include "kludgetinfo.h"

class QWebView;

class InstallWindow : public PreferenceWindow
{
    Q_OBJECT
public:
    InstallWindow(const KludgetInfo& info);

    void createHeader();
    void createDialogControls();

    private
Q_SLOTS:
    void onUpdate();

private:
    KludgetInfo info;
    QWebView *view;
};

class AboutWindow : public InstallWindow
{
    Q_OBJECT
public:
    AboutWindow(const KludgetInfo& info);
    void createDialogControls();

    private
Q_SLOTS:
    void onSecurity();

};

class AboutKludgetWindow : public PreferenceWindow
{
    Q_OBJECT
public:

    AboutKludgetWindow();

    void createHeader();
    void createToolbar() {}
    void createPages() {}
    void createDialogControls();

    private
Q_SLOTS:
    void onWebSite();
    void onCredits();

private:
    QWebView *view;
};

#endif
