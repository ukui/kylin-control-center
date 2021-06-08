#-------------------------------------------------
#
# Project created by QtCreator 2019-06-25T12:51:26
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT       += widgets dbus

TEMPLATE = lib
CONFIG   += plugin

TARGET = $$qtLibraryTarget(fonts)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

##加载gio库和gio-unix库，用于获取gsettings的默认值
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS += -L$$[QT_INSTALL_LIBS] -lgsettings-qt

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        fonts.cpp

HEADERS += \
        fonts.h

FORMS += \
        fonts.ui
