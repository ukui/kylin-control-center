#-------------------------------------------------
#
# Project created by QtCreator 2019-06-25T13:24:04
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)
include($$PROJECT_COMPONENTSOURCE/combobox.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

QT       += widgets dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(screensaver)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

DEFINES += QT_INSTALL_LIBS='\\"$$[QT_INSTALL_LIBS]\\"'

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L$$[QT_INSTALL_LIBS] -lgsettings-qt

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        presslabel.cpp \
        screensaver.cpp

HEADERS += \
        presslabel.h \
        screensaver.h

FORMS += \
        screensaver.ui
