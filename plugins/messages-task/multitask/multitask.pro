#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T15:22:24
#
#-------------------------------------------------

include(../../../env.pri)
include(../../pluginsComponent/pluginsComponent.pri)

QT       += widgets

TARGET = $$qtLibraryTarget(multitask)
DESTDIR = ../..
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

#DEFINES += QT_DEPRECATED_WARNINGS

target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

SOURCES += \
        multitask.cpp

HEADERS += \
        multitask.h

FORMS += \
        multitask.ui