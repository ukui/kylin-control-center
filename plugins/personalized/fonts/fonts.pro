#-------------------------------------------------
#
# Project created by QtCreator 2019-06-25T12:51:26
#
#-------------------------------------------------

QT       += widgets

TEMPLATE = lib
CONFIG   += plugin

TARGET = $$qtLibraryTarget(fonts)
DESTDIR = ../../../pluginlibs


##加载gio库和gio-unix库，用于获取gsettings的默认值
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS += -L/usr/lib/ -lgsettings-qt

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        fonts.cpp

HEADERS += \
        fonts.h

FORMS += \
        fonts.ui
