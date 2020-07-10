#-------------------------------------------------
#
# Project created by QtCreator 2019-06-26T10:59:46
#
#-------------------------------------------------

QT       += widgets x11extras

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(desktop)
DESTDIR = ../../../pluginlibs

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \
#                 xdamage \
#                 xrender \
#                 xcomposite\

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf \


LIBS          += -L/usr/lib/ -lgsettings-qt -ldconf

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        desktop.cpp \
        realizedesktop.cpp

HEADERS += \
        desktop.h \
        realizedesktop.h

FORMS += \
        desktop.ui
