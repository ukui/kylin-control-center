#-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T15:36:43
#
#-------------------------------------------------
QT            += widgets core gui quickwidgets quick xml #KScreen KI18n KConfigCore KConfigWidgets KWidgetsAddons
TEMPLATE = lib
CONFIG        += plugin

TARGET = $$qtLibraryTarget(display)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

INCLUDEPATH   +=  \
                /usr/include/KF5/ \
                /usr/include/KF5/KWidgetsAddons \
                /usr/include/KF5/KCoreAddons \

LIBS          += -L/usr/lib/ -lgsettings-qt


SOURCES += \
        display.cpp \
    declarative/qmloutput.cpp \
    declarative/qmloutputcomponent.cpp \
    declarative/qmlscreen.cpp \
    controlpanel.cpp \
    outputconfig.cpp \
    resolutionslider.cpp \
    unifiedoutputconfig.cpp \
    utils.cpp \
    widget.cpp

HEADERS += \
        display.h \
    declarative/qmloutput.h \
    declarative/qmloutputcomponent.h \
    declarative/qmlscreen.h \
    controlpanel.h \
    outputconfig.h \
    resolutionslider.h \
    unifiedoutputconfig.h \
    utils.h \
    widget.h

FORMS += \
    display.ui

#DISTFILES += \
#    qml/main.qml \
#    qml/Output.qml

RESOURCES += \
    qml.qrc

DISTFILES +=
