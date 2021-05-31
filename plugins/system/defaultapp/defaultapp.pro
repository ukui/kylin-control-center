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

include($$PROJECT_COMPONENTSOURCE/label.pri)

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \
                 $$PROJECT_COMPONENTSOURCE
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
