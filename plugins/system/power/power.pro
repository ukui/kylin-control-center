include(../../../env.pri)
QT       += widgets dbus
TEMPLATE = lib
CONFIG   += plugin

include($$PROJECT_COMPONENTSOURCE/comboxframe.pri)

TARGET = $$qtLibraryTarget(power)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH += \
               $$PROJECT_ROOTDIR \

LIBS     += -L$$[QT_INSTALL_LIBS] -lgsettings-qt

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
