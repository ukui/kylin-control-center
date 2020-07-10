QT       += widgets
TEMPLATE = lib
CONFIG   += plugin

TARGET = $$qtLibraryTarget(power)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)

INCLUDEPATH += \
               $$PROJECT_ROOTDIR \


LIBS     += -L/usr/lib/ -lgsettings-qt

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \


FORMS += \
    power.ui

HEADERS += \
    power.h \
    powermacrodata.h

SOURCES += \
    power.cpp
