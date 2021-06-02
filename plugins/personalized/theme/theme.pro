#-------------------------------------------------
#
# Project created by QtCreator 2019-06-21T10:50:19
#
#-------------------------------------------------
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT       += widgets KConfigCore KI18n x11extras concurrent dbus

TEMPLATE = lib
CONFIG += plugin \
          link_pkgconfig \
          c++11
TARGET = $$qtLibraryTarget(theme)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lX11 -lXfixes -lXcursor

PKGCONFIG     += gsettings-qt

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

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
