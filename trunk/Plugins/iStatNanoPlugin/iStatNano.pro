
TEMPLATE = lib
TARGET =  
DEPENDPATH += .
INCLUDEPATH += .

QT += webkit

win32* {
    DESTDIR = "C:\Documents and Settings\marvin\Local Settings\Application Data\MindHyve\Kludgets\widgets\installed\istatnano\iStat nano.wdgt\iStatNano.bundle\Contents\Windows"
    LIBS += Setupapi.lib
}

# Input
HEADERS += plugin.h \
            dataminers\data.h \
            dataminers\cpu.h \
            dataminers\disk.h \
            dataminers\mem.h \
            dataminers\net.h \
            dataminers\bat.h \
            dataminers\perfmon.h
SOURCES += plugin.cpp \
            dataminers\data.cpp \
            dataminers\cpu.cpp \
            dataminers\disk.cpp \
            dataminers\mem.cpp \
            dataminers\net.cpp \
            dataminers\bat.cpp \
            dataminers\perfmon.cpp

RESOURCES += resources.qrc
