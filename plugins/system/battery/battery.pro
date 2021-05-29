include(../../../env.pri)
QT       += widgets dbus
TEMPLATE = lib
CONFIG   += plugin

include($$PROJECT_COMPONENTSOURCE/comboxframe.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

TARGET = $$qtLibraryTarget(battery)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \

LIBS     += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lupower-glib

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \
             gio-2.0 \
             gio-unix-2.0 \
             upower-glib

HEADERS += \
    ../power/powermacrodata.h \
    battery.h \

SOURCES += \
    battery.cpp

FORMS +=
