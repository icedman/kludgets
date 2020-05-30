TEMPLATE = app
TARGET = kludgets
DEPENDPATH =
INCLUDEPATH += .

QT += webkit webkitwidgets network xml xmlpatterns widgets x11extras 

win32* {
    LIBS += gdi32.lib user32.lib advapi32.lib
    HEADERS += win/config_win.h \
               win/windowsregistry.h
    SOURCES += win/khotkey_win.cpp \
               win/kipc_win.cpp \
               win/kwindow_win.cpp \
               win/kserver_win.cpp \
               win/kpluginview_win.cpp \
               win/windowsregistry.cpp

    DESTDIR = ./
    RC_FILE = app.rc
}

!win32* {
    SOURCES += linux/khotkey_linux.cpp \
               linux/kipc_linux.cpp \
               linux/kwindow_linux.cpp \
               linux/kserver_linux.cpp \
               linux/kpluginview_linux.cpp
}

HEADERS += config.h \
           installwindow.h \
           kapp.h \
           kclient.h \
           kdocument.h \
           khotkey.h \
           khudwindow.h \
           kipc.h \
           klocalefilelist.h \
           klog.h \
           kludget.h \
           kludgetinfo.h \
           knetwork.h \
           kpage.h \
           kpluginview.h \
           kserver.h \
           ksettings.h \
           ksystem.h \
           kutil.h \
           kview.h \
           kwindow.h \
           prefwindow.h \
           version.h
           
SOURCES += installwindow.cpp \
           kapp.cpp \
           kclient.cpp \
           kdocument.cpp \
           khotkey.cpp \
           khudwindow.cpp \
           kipc.cpp \
           klocalefilelist.cpp \
           klog.cpp \
           kludget.cpp \
           kludgetinfo.cpp \
           knetwork.cpp \
           kpage.cpp \
           kpluginview.cpp \
           kserver.cpp \
           ksettings.cpp \
           ksystem.cpp \
           kutil.cpp \
           kview.cpp \
           kwindow.cpp \
           main.cpp \
           prefwindow.cpp
           
RESOURCES += resources.qrc
