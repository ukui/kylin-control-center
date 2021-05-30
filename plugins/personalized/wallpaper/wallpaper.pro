#-------------------------------------------------
#
# Project created by QtCreator 2019-06-17T11:16:02
#
#-------------------------------------------------
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/flowlayout.pri)
include($$PROJECT_COMPONENTSOURCE/maskwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

QT       += widgets xml dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(wallpaper)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \
                 $$PROJECT_COMPONENTSOURCE \

LIBS += -L$$[QT_INSTALL_LIBS] -lgsettings-qt

##加载gio库和gio-unix库，用于获取和设置enum类型的gsettings
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    colordialog.cpp \
    colorpreview.cpp \
    colorsquare.cpp \
    gradientslider.cpp \
    pictureunit.cpp \
    wallpaper.cpp \
    xmlhandle.cpp \
    component/custdomitemmodel.cpp \
    simplethread.cpp \
    workerobject.cpp

HEADERS += \
    colordialog.h \
    colorpreview.h \
    colorsquare.h \
    gradientslider.h \
    pictureunit.h \
    wallpaper.h \
    xmlhandle.h \
    component/custdomitemmodel.h \
    simplethread.h \
    workerobject.h

FORMS += \
    colordialog.ui \
    wallpaper.ui
