
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT += webkit network xml svg xmlpatterns

win32* {
    LIBS += gdi32.lib user32.lib advapi32.lib
    HEADERS += win\config_win.h \
               win\windowsregistry.h
    SOURCES += win\kwindow_win.cpp \
               win\kserver_win.cpp \
               win\kpluginview_win.cpp \
               win\windowsregistry.cpp
    DESTDIR = ./
}

HEADERS += config.h \
           kdocument.h \
           kapp.h \
           kserver.h \
           kludget.h \
           kludgetinfo.h \
           kclient.h \
           kwindow.h \
           kview.h \
           kpluginview.h \
           kpage.h \
           knetwork.h \
           ksettings.h \
           ksystem.h \
           prefwindow.h \
           installwindow.h \
           version.h \
           localefilelist.h \
           util.h
           
SOURCES += kdocument.cpp \
           kapp.cpp \
           kserver.cpp \
           kludget.cpp \
           kludgetinfo.cpp \
           kclient.cpp \
           kwindow.cpp \
           kview.cpp \
           kpluginview.cpp \
           kpage.cpp \
           knetwork.cpp \
           ksettings.cpp \
           ksystem.cpp \
           prefwindow.cpp \
           installwindow.cpp \
           localefilelist.cpp \
           util.cpp \
           main.cpp

RESOURCES += resources.qrc

