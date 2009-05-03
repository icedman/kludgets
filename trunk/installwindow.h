#ifndef INSTALLWINDOW_H
#define INSTALLWINDOW_H

#include "prefwindow.h"
#include "kludgetinfo.h"

class InstallWindow : public PreferenceWindow
{
    Q_OBJECT
public:
    InstallWindow(const KludgetInfo& info);

    void createHeader();
    void createDialogControls();

private:
    KludgetInfo info;
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

#endif
