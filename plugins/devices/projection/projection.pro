#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:09:42
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

QT       += widgets
QT       += core gui widgets dbus

TEMPLATE = lib
CONFIG += plugin
CONFIG += link_pkgconfig
PKGCONFIG += gsettings-qt

TARGET = $$qtLibraryTarget(projection)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \

INCLUDEPATH += /usr/lib/gcc/aarch64-linux-gnu/9/include/


#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        projection.cpp \
        changeprojectionname.cpp

HEADERS += \
        projection.h \
        changeprojectionname.h

FORMS += \
        projection.ui \
        changeprojectionname.ui \

INSTALLS += target

RESOURCES += \
    pic.qrc
