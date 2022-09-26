#-------------------------------------------------
#
# Project created by QtCreator 2019-10-29T13:22:33
#
#-------------------------------------------------

QT += core gui network xml webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Prser
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        SearchHighLight.cpp \
        chromiumwebserf.cpp \
        customregexp.cpp \
        downloads.cpp \
        fileoperations.cpp \
        main.cpp \
        parsejavascript.cpp \
        parsejssettingsgui.cpp \
        parser.cpp \
        parsesetting.cpp \
        parsetargetgui.cpp \
        parsetargetsite.cpp \
        variousstrvalidations.cpp

HEADERS += \
        SearchHighLight.h \
        chromiumwebserf.h \
        customregexp.h \
        downloads.h \
        fileoperations.h \
        parsejavascript.h \
        parsejssettingsgui.h \
        parser.h \
        parsesetting.h \
        parsetargetgui.h \
        parsetargetsite.h \
        variousstrvalidations.h

FORMS += \
        chromiumwebserf.ui \
        customregexp.ui \
        parsejssettingsgui.ui \
        parser.ui \
        parsesetting.ui \
        parsetargetgui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
