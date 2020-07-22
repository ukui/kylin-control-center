#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

include(../../../env.pri)
include(../../pluginsComponent/pluginsComponent.pri)

QT       += widgets x11extras

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(mousecontrol)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../.. \
                /usr/include/kylinsdk \

LIBS          +=  -L$$[QT_INSTALL_LIBS] -lmouseclient  -ltouchpadclient -lXi -lgsettings-qt

CONFIG += link_pkgconfig c++11

PKGCONFIG += gsettings-qt

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    mousecontrol.cpp

HEADERS += \
    mousecontrol.h

FORMS += \
    mousecontrol.ui

INSTALLS += target
