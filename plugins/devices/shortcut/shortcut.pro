QT       += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(shortcut)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf

LIBS          +=  -L/usr/lib/ -lgsettings-qt -ldconf

CONFIG +=  \
          link_pkgconfig \
          c++11

PKGCONFIG += gio-2.0 \
             gio-unix-2.0 \
             gsettings-qt \

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addshortcutdialog.cpp \
    customlineedit.cpp \
    defineshortcutitem.cpp \
    getshortcutworker.cpp \
    keymap.cpp \
    realizeshortcutwheel.cpp \
    shortcut.cpp \
    showallshortcut.cpp

HEADERS += \
    addshortcutdialog.h \
    customlineedit.h \
    defineshortcutitem.h \
    getshortcutworker.h \
    keymap.h \
    realizeshortcutwheel.h \
    shortcut.h \
    showallshortcut.h

FORMS += \
    addshortcutdialog.ui \
    shortcut.ui \
    showallshortcut.ui
