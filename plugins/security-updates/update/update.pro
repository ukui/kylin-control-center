#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T11:47:23
#
#-------------------------------------------------

include(../../../env.pri)

QT       += widgets

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(update)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    update.cpp

HEADERS += \
    update.h

FORMS += \
    update.ui
