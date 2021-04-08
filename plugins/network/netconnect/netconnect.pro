#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T13:45:31
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverbtn.pri)

QT       += widgets network dbus gui core
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(netconnect)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt

CONFIG += c++11 \
          link_pkgconfig \

PKGCONFIG += gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    netconnect.cpp \
    netconnectwork.cpp \
    netdetail.cpp

HEADERS += \
    netconnect.h \
    netconnectwork.h \
    netdetail.h

FORMS += \
    netconnect.ui

INSTALLS += target
