include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

QT       += widgets dbus KI18n KCoreAddons

TEMPLATE = lib
CONFIG += plugin \
          link_pkgconfig

PKGCONFIG     += gsettings-qt


TARGET = $$qtLibraryTarget(about)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \
                 $$PROJECT_COMPONENTSOURCE
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
