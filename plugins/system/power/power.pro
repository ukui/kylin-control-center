include(../../../env.pri)
QT       += widgets dbus
TEMPLATE = lib
CONFIG   += plugin

include($$PROJECT_COMPONENTSOURCE/comboxframe.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

TARGET = $$qtLibraryTarget(power)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH += \
               $$PROJECT_ROOTDIR \
               $$PROJECT_COMPONENTSOURCE

LIBS     += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lupower-glib

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \
             gio-2.0 \
             gio-unix-2.0 \
             upower-glib

FORMS += \
    power.ui

HEADERS += \
    power.h \
    powermacrodata.h

SOURCES += \
    power.cpp
