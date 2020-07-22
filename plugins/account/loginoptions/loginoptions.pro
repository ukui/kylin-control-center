#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T14:44:56
#
#-------------------------------------------------

QT       += widgets

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

include(../../pluginsComponent/pluginsComponent.pri)
include(../../../env.pri)

TARGET = $$qtLibraryTarget(loginoptions)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        loginoptions.cpp

HEADERS += \
        loginoptions.h

FORMS += \
        loginoptions.ui

INSTALLS += target