#-------------------------------------------------
#
# Project created by QtCreator 2020-02-26T16:15:07
#
#-------------------------------------------------

QT       += widgets x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG   += plugin

TARGET = $$qtLibraryTarget(touchpad)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

#LIBS          +=  -L/usr/lib/ -ltouchpadclient -lXi -lgsettings-qt
LIBS          +=  -L/usr/lib/  -lXi -lgsettings-qt

CONFIG        += link_pkgconfig \
                 C++11

PKGCONFIG     += gsettings-qt \

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        touchpad.cpp

HEADERS += \
        touchpad.h

FORMS += \
        touchpad.ui
