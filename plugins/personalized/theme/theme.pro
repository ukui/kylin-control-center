#-------------------------------------------------
#
# Project created by QtCreator 2019-06-21T10:50:19
#
#-------------------------------------------------
QT       += widgets KConfigCore KI18n x11extras concurrent

TEMPLATE = lib
CONFIG += plugin


TARGET = $$qtLibraryTarget(theme)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L/usr/lib/ -lgsettings-qt -lX11 -lXfixes -lXcursor


CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    cursor/cursortheme.cpp \
    cursor/xcursortheme.cpp \
        theme.cpp \
    themewidget.cpp \
    widgetgroup.cpp \
    ../../../shell/customstyle.cpp

HEADERS += \
    cursor/config-X11.h \
    cursor/cursortheme.h \
    cursor/xcursortheme.h \
        theme.h \
    themewidget.h \
    widgetgroup.h \
    ../../../shell/customstyle.h

FORMS += \
        theme.ui

RESOURCES +=
