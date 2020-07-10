#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T13:59:06
#
#-------------------------------------------------

QT       += widgets

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(proxy)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L/usr/lib/ -lgsettings-qt

##加载gio库和gio-unix库，用于获取和设置enum类型的gsettings
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        proxy.cpp \
    certificationdialog.cpp

HEADERS += \
        proxy.h \
    certificationdialog.h

FORMS += \
        proxy.ui \
    certificationdialog.ui
