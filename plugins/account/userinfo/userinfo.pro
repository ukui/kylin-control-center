#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T14:35:43
#
#-------------------------------------------------
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/flowlayout.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

QT       += widgets dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(userinfo)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L$$[QT_INSTALL_LIBS] -lcrypt

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    changevaliddialog.cpp \
    elipsemaskwidget.cpp \
    userinfo.cpp \
    qtdbus/systemdbusdispatcher.cpp \
    changepwddialog.cpp \
    qtdbus/userdispatcher.cpp \
    changetypedialog.cpp \
    changefacedialog.cpp \
    deluserdialog.cpp \
    createuserdialog.cpp \
    passwdcheckutil.cpp

HEADERS += \
    changevaliddialog.h \
    elipsemaskwidget.h \
    userinfo.h \
    qtdbus/systemdbusdispatcher.h \
    changepwddialog.h \
    qtdbus/userdispatcher.h \
    changetypedialog.h \
    changefacedialog.h \
    deluserdialog.h \
    createuserdialog.h \
    passwdcheckutil.h

FORMS += \
    changevaliddialog.ui \
    userinfo.ui \
    changepwddialog.ui \
    changetypedialog.ui \
    changefacedialog.ui \
    deluserdialog.ui \
    createuserdialog.ui

INSTALLS += target
