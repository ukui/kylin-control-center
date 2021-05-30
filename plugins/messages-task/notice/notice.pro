QT       += widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/closebutton.pri)

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(notice)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
#                 Qt5X

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf

LIBS          +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt -ldconf


CONFIG += c++11 \
          link_pkgconfig \

PKGCONFIG += gsettings-qt \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appdetail.cpp \
    notice.cpp \
    realizenotice.cpp

HEADERS += \
    appdetail.h \
    notice.h \
    realizenotice.h

FORMS += \
    appdetail.ui \
    notice.ui

INSTALLS += target
