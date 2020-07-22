#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T11:56:39
#
#-------------------------------------------------
include(../../../env.pri)
include(../../pluginsComponent/pluginsComponent.pri)

QT       += widgets

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

TARGET = $$qtLibraryTarget(recovery)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        recovery.cpp

HEADERS += \
        recovery.h

FORMS += \
        recovery.ui
