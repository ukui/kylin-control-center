#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T14:35:43
#
#-------------------------------------------------

QT       += widgets dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(userinfo)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/flowlayout.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L/usr/lib/ -lcrypt

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    elipsemaskwidget.cpp \
        userinfo.cpp \
    qtdbus/systemdbusdispatcher.cpp \
    changepwddialog.cpp \
    qtdbus/userdispatcher.cpp \
    changetypedialog.cpp \
    changefacedialog.cpp \
    deluserdialog.cpp \
    createuserdialog.cpp

HEADERS += \
    elipsemaskwidget.h \
        userinfo.h \
    qtdbus/systemdbusdispatcher.h \
    changepwddialog.h \
    qtdbus/userdispatcher.h \
    changetypedialog.h \
    changefacedialog.h \
    deluserdialog.h \
    createuserdialog.h

FORMS += \
        userinfo.ui \
    changepwddialog.ui \
    changetypedialog.ui \
    changefacedialog.ui \
    deluserdialog.ui \
    createuserdialog.ui

RESOURCES += \
