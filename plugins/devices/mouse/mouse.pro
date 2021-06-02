#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT       += widgets x11extras dbus

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(mouse)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          +=  -L$$[QT_INSTALL_LIBS]  -lXi -lgsettings-qt
#LIBS          +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \
                 x11

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        mousecontrol.cpp

HEADERS += \
        mousecontrol.h

FORMS += \
        mousecontrol.ui

INSTALLS += target
