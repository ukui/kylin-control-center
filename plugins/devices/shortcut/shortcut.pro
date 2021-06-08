include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

QT       += widgets dbus KXmlGui KGlobalAccel
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(shortcut)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \
               /usr/include/dconf

LIBS        += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -ldconf

CONFIG += link_pkgconfig c++11

PKGCONFIG += gio-2.0 \
             gio-unix-2.0 \
             gsettings-qt \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addshortcutdialog.cpp \
    customlineedit.cpp \
    defineshortcutitem.cpp \
    getshortcutworker.cpp \
    keymap.cpp \
    realizeshortcutwheel.cpp \
    shortcut.cpp

HEADERS += \
    addshortcutdialog.h \
    customlineedit.h \
    defineshortcutitem.h \
    getshortcutworker.h \
    keymap.h \
    realizeshortcutwheel.h \
    shortcut.h

FORMS += \
    addshortcutdialog.ui \
    shortcut.ui

INSTALLS += target
