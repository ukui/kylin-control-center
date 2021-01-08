

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

QT += widgets core gui quickwidgets quick xml KScreen KI18n KConfigCore KConfigWidgets KWidgetsAddons dbus
TEMPLATE = lib
CONFIG += c++11 link_pkgconfig plugin

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

UI_DIR=./

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = $$qtLibraryTarget(touchscreen)
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


SOURCES += \
    declarative/qmloutput.cpp \
    declarative/qmloutputcomponent.cpp \
    declarative/qmlscreen.cpp \
    monitorinputtask.cpp \
    touchscreen.cpp \
    utils.cpp \
    widget.cpp \
    xinputmanager.cpp

HEADERS += \
    declarative/qmloutput.h \
    declarative/qmloutputcomponent.h \
    declarative/qmlscreen.h \
    monitorinputtask.h \
    touchscreen.h \
    utils.h \
    widget.h \
    xinputmanager.h

FORMS += \
    touchscreen.ui

RESOURCES += \
    qml.qrc


