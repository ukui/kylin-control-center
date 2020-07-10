#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

QT       += widgets x11extras
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(keyboard)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          +=  -L/usr/lib/ -lgsettings-qt

CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += libmatekbd \
                 gsettings-qt \


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        keyboardcontrol.cpp \
    kbdlayoutmanager.cpp

HEADERS += \
        keyboardcontrol.h \
    kbdlayoutmanager.h

FORMS += \
        keyboardcontrol.ui \
    kbdlayoutmanager.ui
