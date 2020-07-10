#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

QT       += widgets x11extras

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(mouse)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          +=  -L/usr/lib/  -lXi -lgsettings-qt
#LIBS          +=  -L/usr/lib/   -lgsettings-qt

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        mousecontrol.cpp

HEADERS += \
        mousecontrol.h

FORMS += \
        mousecontrol.ui
