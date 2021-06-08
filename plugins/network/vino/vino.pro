include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets dbus

TEMPLATE = lib
CONFIG += plugin link_pkgconfig
PKGCONFIG += gsettings-qt

TARGET = $$qtLibraryTarget(vino)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE\
                 $$PROJECT_ROOTDIR \

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

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
    sharemain.cpp \
    vino.cpp

HEADERS += \
    sharemain.h \
    vino.h

FORMS +=

# Default rules for deployment.
INSTALLS += target
