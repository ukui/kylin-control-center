QT       += core gui svg dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../../../env.pri)

TEMPLATE = lib
CONFIG += c++11 plugin
TARGET = $$qtLibraryTarget(networkaccount)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
    $$PROJECT_ROOTDIR \

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
    areacodelineedit.cpp \
    bindphonedialog.cpp \
    blueeffect.cpp \
    boxitem.cpp \
    combobox.cpp \
    configfile.cpp \
    dbushandleclient.cpp \
    editpassdialog.cpp \
    editpushbutton.cpp \
    frameitem.cpp \
    infolabel.cpp \
    itemlist.cpp \
    logindialog.cpp \
    maindialog.cpp \
    mainwidget.cpp \
    mcodewidget.cpp \
    networkaccount.cpp \
    passdialog.cpp \
    passwordlineedit.cpp \
    popupwidget.cpp \
    regdialog.cpp \
    successdiaolog.cpp \
    svghandler.cpp \
    switchbutton.cpp \
    tips.cpp \
    tooltips.cpp

HEADERS += \
    areacodelineedit.h \
    bindphonedialog.h \
    blueeffect.h \
    boxitem.h \
    combobox.h \
    configfile.h \
    dbushandleclient.h \
    editpassdialog.h \
    editpushbutton.h \
    frameitem.h \
    infolabel.h \
    itemlist.h \
    logindialog.h \
    maindialog.h \
    mainwidget.h \
    mcodewidget.h \
    networkaccount.h \
    passdialog.h \
    passwordlineedit.h \
    popupwidget.h \
    regdialog.h \
    successdiaolog.h \
    svghandler.h \
    switchbutton.h \
    tips.h \
    tooltips.h

FORMS += \

RESOURCES += \
    image/Delete.qrc \
    json/jsonFile.qrc \
    qss.qrc

INSTALLS += target
