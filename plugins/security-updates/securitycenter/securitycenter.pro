include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/flowlayout.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

QT       += widgets dbus

TEMPLATE = lib
CONFIG += plugin \
          link_pkgconfig
PKGCONFIG     += gsettings-qt

TARGET = $$qtLibraryTarget(securitycenter)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \
                 $$PROJECT_COMPONENTSOURCE \

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    defender_interface.cpp \
    fontwatcher.cpp \
    ksc_main_page_widget.cpp \
    ksc_module_func_widget.cpp \
    ksc_set_font_size.cpp \
    securitycenter.cpp

HEADERS += \
    defender_interface.h \
    fontwatcher.h \
    ksc_business_def.h \
    ksc_main_page_widget.h \
    ksc_module_func_widget.h \
    ksc_set_font_size.h \
    securitycenter.h

FORMS += \
    ksc_main_page_widget.ui \
    ksc_module_func_widget.ui
