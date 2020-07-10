QT       += widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(notice)
DESTDIR = ../../../pluginlibs

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
#                 Qt5Xdg


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
                 /usr/include/dconf

LIBS          +=  -L/usr/lib/ -lgsettings-qt -ldconf


CONFIG += c++11 \
          link_pkgconfig \

PKGCONFIG += gsettings-qt \

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
