#-------------------------------------------------
#
# Project created by QtCreator 2019-08-22T11:12:59
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/uslider.pri)

QT       += widgets x11extras KWindowSystem xml KGuiAddons KCoreAddons concurrent KConfigCore KConfigGui KI18n
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(keyboard)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/KF5 \
                 /usr/include/xcb \

LIBS          +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lX11 -lxkbfile

CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += libmatekbd \
                 gsettings-qt \


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    keyboardcontrol.cpp \
    kbdlayoutmanager.cpp \
 \#    tastenbrett.cpp
    preview/debug.cpp \
    preview/geometry_components.cpp \
    preview/geometry_parser.cpp \
    preview/kbpreviewframe.cpp \
    preview/keyaliases.cpp \
    preview/keyboard_config.cpp \
    preview/keyboardlayout.cpp \
    preview/keyboardpainter.cpp \
    preview/keysym2ucs.cpp \
    preview/keysymhelper.cpp \
    preview/symbol_parser.cpp \
    preview/x11_helper.cpp \
    preview/xkb_rules.cpp

HEADERS += \
    keyboardcontrol.h \
    kbdlayoutmanager.h \
 \#    tastenbrett.h
    preview/config-keyboard.h \
    preview/config-workspace.h \
    preview/debug.h \
    preview/geometry_components.h \
    preview/geometry_parser.h \
    preview/kbpreviewframe.h \
    preview/keyaliases.h \
    preview/keyboard_config.h \
    preview/keyboardlayout.h \
    preview/keyboardpainter.h \
    preview/keysym2ucs.h \
    preview/keysymhelper.h \
    preview/symbol_parser.h \
    preview/x11_helper.h \
    preview/xkb_rules.h

FORMS += \
    keyboardcontrol.ui \
    kbdlayoutmanager.ui \
    layoutmanager.ui

INSTALLS += target
