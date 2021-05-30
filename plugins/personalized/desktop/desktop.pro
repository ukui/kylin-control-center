#-------------------------------------------------
#
# Project created by QtCreator 2019-06-26T10:59:46
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/listdelegate.pri)

QT       += widgets x11extras dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(desktop)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \
#                 xdamage \
#                 xrender \
#                 xcomposite

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf \

LIBS          += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -ldconf

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        desktop.cpp \
        realizedesktop.cpp

HEADERS += \
        desktop.h \
        realizedesktop.h

FORMS += \
        desktop.ui

INSTALLS += target
