#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T15:14:42
#
#-------------------------------------------------

QT       += widgets dbus KWidgetsAddons

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

TARGET = $$qtLibraryTarget(area)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

LIBS += -L$$[QT_INSTALL_DIRS] -lgsettings-qt

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \

##加载gio库和gio-unix库，用于处理时间
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        area.cpp \
        dataformat.cpp

HEADERS += \
        area.h \
        dataformat.h

FORMS += \
        area.ui \
        dataformat.ui
