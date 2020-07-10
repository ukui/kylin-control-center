#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T15:14:42
#
#-------------------------------------------------

QT       += widgets dbus KWidgetsAddons

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

TARGET = $$qtLibraryTarget(area)
DESTDIR = ../../../pluginlibs

LIBS += -L/usr/lib/ -lgsettings-qt


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \


##加载gio库和gio-unix库，用于处理时间
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \


#DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        area.cpp \
        dataformat.cpp

HEADERS += \
        area.h \
        dataformat.h

FORMS += \
        area.ui \
        dataformat.ui
