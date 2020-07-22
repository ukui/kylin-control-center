include(../../../env.pri)

QT       += widgets dbus KI18n KCoreAddons

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(about)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

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

INSTALLS += target