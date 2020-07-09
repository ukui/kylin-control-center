#-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T15:36:43
#
#-------------------------------------------------
QT            += widgets core gui quickwidgets quick xml KScreen KI18n KConfigCore KConfigWidgets KWidgetsAddons dbus
TEMPLATE = lib
CONFIG        += plugin

TARGET = $$qtLibraryTarget(display)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L/usr/lib/ -lgsettings-qt

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gsettings-qt \

SOURCES += \
        display.cpp \
    declarative/qmloutput.cpp \
    declarative/qmloutputcomponent.cpp \
    declarative/qmlscreen.cpp \
    controlpanel.cpp \
    outputconfig.cpp \
    resolutionslider.cpp \
    slider.cpp \
    unifiedoutputconfig.cpp \
    utils.cpp \
    widget.cpp \
    displayperformancedialog.cpp

HEADERS += \
        display.h \
    declarative/qmloutput.h \
    declarative/qmloutputcomponent.h \
    declarative/qmlscreen.h \
    controlpanel.h \
    outputconfig.h \
    resolutionslider.h \
    slider.h \
    unifiedoutputconfig.h \
    utils.h \
    widget.h \
    displayperformancedialog.h

FORMS += \
    display.ui \
    displayperformancedialog.ui

#DISTFILES += \
#    qml/main.qml \
#    qml/Output.qml

RESOURCES += \
    qml.qrc

DISTFILES +=
