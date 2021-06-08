QT       += core gui svg dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../../../env.pri)

TEMPLATE = lib
CONFIG += c++11 plugin link_pkgconfig
PKGCONFIG += gsettings-qt
TARGET = $$qtLibraryTarget(networkaccount)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

include($$PROJECT_COMPONENTSOURCE/closebutton.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH += \
    $$PROJECT_COMPONENTSOURCE \
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
    blueeffect.cpp \
    configfile.cpp \
    dbusutils.cpp \
    frameitem.cpp \
    itemlist.cpp \
    logindialog.cpp \
    maindialog.cpp \
    mainwidget.cpp \
    mcodewidget.cpp \
    networkaccount.cpp \
    passwordlineedit.cpp \
    svghandler.cpp \
    syncdialog.cpp \
    tips.cpp \
    visblebutton.cpp

HEADERS += \
    blueeffect.h \
    configfile.h \
    dbusutils.h \
    frameitem.h \
    itemlist.h \
    logindialog.h \
    maindialog.h \
    mainwidget.h \
    mcodewidget.h \
    networkaccount.h \
    passwordlineedit.h \
    svghandler.h \
    syncdialog.h \
    tips.h \
    visblebutton.h

FORMS += \

RESOURCES += \
    image/Delete.qrc \
    json/jsonFile.qrc \
    qss.qrc

INSTALLS += target
