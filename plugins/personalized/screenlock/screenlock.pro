#-------------------------------------------------
#
# Project created by QtCreator 2019-06-26T08:25:40
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/flowlayout.pri)
include($$PROJECT_COMPONENTSOURCE/maskwidget.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT       += widgets xml dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(screenlock)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \
                 $$PROJECT_COMPONENTSOURCE \

LIBS += -L$$[QT_INSTALL_LIBS] -lgsettings-qt


##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gsettings-qt \
                 gio-2.0 \
#                 gio-unix-2.0

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        bgfileparse.cpp \
        buildpicunitsworker.cpp \
        pictureunit.cpp \
        screenlock.cpp \
        xmlhandle.cpp

HEADERS += \
        bgfileparse.h \
        buildpicunitsworker.h \
        pictureunit.h \
        screenlock.h \
        xmlhandle.h

FORMS += \
        screenlock.ui
