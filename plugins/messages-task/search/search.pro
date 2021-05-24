QT       += widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/comboxframe.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(search)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf

LIBS          +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt -ldconf


CONFIG += c++11 \
          link_pkgconfig \

PKGCONFIG += gsettings-qt \

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    search.cpp

HEADERS += \
    search.h

FORMS += \
    search.ui

INSTALLS += target

DISTFILES += \
    ../../../commonComponent/imageutil.pri
