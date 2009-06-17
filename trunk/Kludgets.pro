
# CONFIG += debug
TEMPLATE = app
TARGET = 
DEPENDPATH =
INCLUDEPATH += .

QT += webkit network xml svg xmlpatterns

win32* {
    LIBS += gdi32.lib user32.lib advapi32.lib
    HEADERS += win\config_win.h \
               win\windowsregistry.h
    SOURCES += win\kipc_win.cpp \
               win\kwindow_win.cpp \
               win\kserver_win.cpp \
               win\kpluginview_win.cpp \
               win\windowsregistry.cpp \
               win\hotkey_win.cpp
    DESTDIR = ./
    RC_FILE = app.rc
}

!win32* {
    SOURCES += linux\kipc_linux.cpp \
           linux\kwindow_linux.cpp \
           linux\kserver_linux.cpp \
           linux\kpluginview_linux.cpp \
           linux\hotkey_linux.cpp
}

RESOURCES += resources.qrc

HEADERS += config.h \
           hotkey.h \
           hudwindow.h \
           installwindow.h \
           kapp.h \
           kclient.h \
           kdocument.h \
           kipc.h \
           klog.h \
           kludget.h \
           kludgetinfo.h \
           knetwork.h \
           kpage.h \
           kpluginview.h \
           kserver.h \
           ksettings.h \
           ksystem.h \
           kview.h \
           kwindow.h \
           localefilelist.h \
           prefwindow.h \
           util.h \
           version.h

SOURCES += hotkey.cpp \
           hudwindow.cpp \
           installwindow.cpp \
           kapp.cpp \
           kclient.cpp \
           kdocument.cpp \
           kipc.cpp \
           klog.cpp \
           kludget.cpp \
           kludgetinfo.cpp \
           knetwork.cpp \
           kpage.cpp \
           kpluginview.cpp \
           kserver.cpp \
           ksettings.cpp \
           ksystem.cpp \
           kview.cpp \
           kwindow.cpp \
           localefilelist.cpp \
           main.cpp \
           prefwindow.cpp \
           util.cpp
           
