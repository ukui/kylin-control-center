#-------------------------------------------------
#
# Project created by QtCreator 2019-05-30T09:45:54
#
#-------------------------------------------------

QT       += widgets

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../..

TARGET = $$qtLibraryTarget(audio)
DESTDIR = ../../../pluginlibs

CONFIG += c++11 \
          no_keywords link_pkgconfig


PKGCONFIG += libmatemixer \
             Qt5Xdg \
             gio-2.0 \
             libxml-2.0 \
             Qt5Multimedia \
             gsettings-qt \

#DEFINES += QT_DEPRECATED_WARNINGS

target.path = $$[QT_INSTALL_PREFIX]/lib/control-center/plugins/
INSTALLS += target

SOURCES += \
        audio.cpp \
        ukmedia_input_widget.cpp \
        ukmedia_main_widget.cpp \
        ukmedia_output_widget.cpp \
        ukmedia_sound_effects_widget.cpp \
        ukui_custom_style.cpp

HEADERS += \
        audio.h \
        ukmedia_input_widget.h \
        ukmedia_main_widget.h \
        ukmedia_output_widget.h \
        ukmedia_sound_effects_widget.h \
        ukui_custom_style.h

FORMS += \
        audio.ui
