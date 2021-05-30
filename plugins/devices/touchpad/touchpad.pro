#-------------------------------------------------
#
# Project created by QtCreator 2020-02-26T16:15:07
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

QT       += widgets x11extras dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG   += plugin

TARGET = $$qtLibraryTarget(touchpad)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

#LIBS          +=  -L$$[QT_INSTALL_LIBS] -ltouchpadclient -lXi -lgsettings-qt
LIBS          +=  -L$$[QT_INSTALL_LIBS] -lXi -lgsettings-qt

CONFIG        += link_pkgconfig \
                 C++11

PKGCONFIG     += gsettings-qt \
                 xi \
                 x11

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        touchpad.cpp

HEADERS += \
        touchpad.h

FORMS += \
        touchpad.ui

INSTALLS += target
