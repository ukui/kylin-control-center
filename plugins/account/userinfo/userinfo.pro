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
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

QT       += widgets dbus gui

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(userinfo)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L$$[QT_INSTALL_LIBS] -lcrypt -lpolkit-qt5-core-1 -lpam

##加载gio库和gio-unix库
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    changegroupdialog.cpp \
    changevaliddialog.cpp \
    creategroupdialog.cpp \
    definegroupitem.cpp \
    delgroupdialog.cpp \
    editgroupdialog.cpp \
    elipsemaskwidget.cpp \
    pwdcheckthread.cpp \
    run-passwd.cpp \
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
    changegroupdialog.h \
    changevaliddialog.h \
    creategroupdialog.h \
    definegroupitem.h \
    delgroupdialog.h \
    editgroupdialog.h \
    elipsemaskwidget.h \
    loginedusers.h \
    pwdcheckthread.h \
    run-passwd.h \
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
    changegroupdialog.ui \
    changevaliddialog.ui \
    creategroupdialog.ui \
    delgroupdialog.ui \
    editgroupdialog.ui \
    userinfo.ui \
    changepwddialog.ui \
    changetypedialog.ui \
    changefacedialog.ui \
    deluserdialog.ui \
    createuserdialog.ui

INSTALLS += target
