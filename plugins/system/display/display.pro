#-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T15:36:43
#
#-------------------------------------------------
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT            += widgets core gui quickwidgets quick xml KScreen KI18n KConfigCore KConfigWidgets KWidgetsAddons dbus concurrent
TEMPLATE = lib
CONFIG        += c++11   link_pkgconfig plugin

TARGET = $$qtLibraryTarget(display)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L$$[QT_INSTALL_LIBS] -lgsettings-qt

PKGCONFIG += gsettings-qt     \
             gtk+-3.0         \
#             glib-2.0         \
             mate-desktop-2.0 \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

SOURCES += \
    brightnessFrame.cpp \
    display.cpp \
    declarative/qmloutput.cpp \
    declarative/qmloutputcomponent.cpp \
    declarative/qmlscreen.cpp \
    controlpanel.cpp \
    outputconfig.cpp \
    resolutionslider.cpp \
    unifiedoutputconfig.cpp \
    utils.cpp \
    widget.cpp \
    displayperformancedialog.cpp

HEADERS += \
    brightnessFrame.h \
    colorinfo.h \
    display.h \
    declarative/qmloutput.h \
    declarative/qmloutputcomponent.h \
    declarative/qmlscreen.h \
    controlpanel.h \
    outputconfig.h \
    resolutionslider.h \
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
