QT       += widgets dbus KI18n KCoreAddons

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(about)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \

#DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    about.h \
    entry.h \
    memoryentry.h

SOURCES += \
    about.cpp \
    entry.cpp \
    memoryentry.cpp

FORMS += \
    about.ui

RESOURCES += \
    res/img.qrc
