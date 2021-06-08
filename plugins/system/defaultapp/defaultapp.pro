#-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T15:36:43
#
#-------------------------------------------------

include(../../../env.pri)
QT            += widgets dbus concurrent
TEMPLATE = lib
CONFIG        += plugin

TARGET = $$qtLibraryTarget(defaultapp)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2
#LIBS          += -L$$[QT_INSTALL_LIBS] -ldefaultprograms \

SOURCES += \
    defaultapp.cpp \
    addappdialog.cpp \
#    component/custdomcombobox.cpp

HEADERS += \
    defaultapp.h \
    addappdialog.h \
#    component/custdomcombobox.h

FORMS += \
    defaultapp.ui \
    addappdialog.ui
