#-------------------------------------------------
#
# Project created by QtCreator 2019-05-30T09:45:54
#
#-------------------------------------------------

include(../../../env.pri)

QT       += widgets xml

TEMPLATE = lib
CONFIG += plugin

include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH += ../../.. \
               $$PROJECT_COMPONENTSOURCE \

TARGET = $$qtLibraryTarget(audio)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

CONFIG += c++11 \
          no_keywords link_pkgconfig

PKGCONFIG += libmatemixer \
             gio-2.0 \
             libxml-2.0 \
             Qt5Multimedia \
             gsettings-qt \
             libcanberra \
             dconf  \
             libpulse \
             libpulse-mainloop-glib
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    audio.cpp \
    ukmedia_input_widget.cpp \
    ukmedia_main_widget.cpp \
    ukmedia_output_widget.cpp \
    ukmedia_sound_effects_widget.cpp \
    ukui_custom_style.cpp \
    customstyle.cpp \
    ukmedia_slider_tip_label_helper.cpp \
    ukui_list_widget_item.cpp

HEADERS += \
    audio.h \
    ukmedia_input_widget.h \
    ukmedia_main_widget.h \
    ukmedia_output_widget.h \
    ukmedia_sound_effects_widget.h \
    ukui_custom_style.h \
    customstyle.h \
    ukmedia_slider_tip_label_helper.h \
    ukui_list_widget_item.h

FORMS += \
    audio.ui

INSTALLS += target
