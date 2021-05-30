include(../../../env.pri)

QT       += widgets dbus KI18n KCoreAddons

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(about)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2
#DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    about.h

SOURCES += \
    about.cpp

FORMS += \
    about.ui

RESOURCES += \
    res/img.qrc

INSTALLS += target
