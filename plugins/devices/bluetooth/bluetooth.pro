include(../../../env.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets dbus BluezQt

TEMPLATE = lib
CONFIG += plugin \
          += c++11 \
          link_pkgconfig

PKGCONFIG     += gsettings-qt

LIBS += -L /usr/lib/x86_64-linux-gnu -l KF5BluezQt

TARGET = $$qtLibraryTarget(bluetooth)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#           QT_NO_WARNING_OUTPUT \
#           QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bluetooth.cpp \
    bluetoothmain.cpp \
    bluetoothnamelabel.cpp \
    deviceinfoitem.cpp \
    loadinglabel.cpp
#    mylayout.cpp

HEADERS += \
    bluetooth.h \
    bluetoothmain.h \
    bluetoothnamelabel.h \
    config.h \
    deviceinfoitem.h \
    loadinglabel.h
#    mylayout.h

# Default rules for deployment.
INSTALLS += target

FORMS +=

