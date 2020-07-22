#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:09:42
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

QT       += widgets printsupport

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(printer)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        printer.cpp

HEADERS += \
        printer.h

FORMS += \
        printer.ui

INSTALLS += target
