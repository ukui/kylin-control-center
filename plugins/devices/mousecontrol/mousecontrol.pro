#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

QT       += widgets x11extras

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(mousecontrol)
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../.. \
                /usr/include/kylinsdk \

LIBS          +=  -L/usr/lib/ -lmouseclient  -ltouchpadclient -lXi -lgsettings-qt



DESTDIR = ../..

include(../../pluginsComponent/pluginsComponent.pri)

target.path = $$[QT_INSTALL_PREFIX]/lib/control-center/plugins/
INSTALLS += target

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        mousecontrol.cpp

HEADERS += \
        mousecontrol.h

FORMS += \
        mousecontrol.ui
