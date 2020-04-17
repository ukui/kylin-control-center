QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = lib
CONFIG += c++11
CONFIG += plugin
TARGET = $$qtLibraryTarget(networkaccount)
DESTDIR = ../../../pluginlibs

include(../../../env.pri)

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
    area_code_lineedit.cpp \
    config_file.cpp \
    config_list_widget.cpp \
    dialog_login_reg.cpp \
    editpassdialog.cpp \
    item_list.cpp \
    logindialog.cpp \
    mcode_widget.cpp \
    network_item.cpp \
    networkaccount.cpp \
    passdialog.cpp \
    ql_box_item.cpp \
    ql_combobobx.cpp \
    ql_popup.cpp \
    ql_pushbutton_edit.cpp \
    ql_swichbutton.cpp \
    qtooltips.cpp \
    regdialog.cpp \
    successdiaolog.cpp

HEADERS += \
    area_code_lineedit.h \
    config_file.h \
    config_list_widget.h \
    dialog_login_reg.h \
    editpassdialog.h \
    item_list.h \
    logindialog.h \
    mcode_widget.h \
    network_item.h \
    networkaccount.h \
    passdialog.h \
    ql_box_item.h \
    ql_combobobx.h \
    ql_popup.h \
    ql_pushbutton_edit.h \
    ql_swichbutton.h \
    qtooltips.h \
    regdialog.h \
    successdiaolog.h

FORMS += \

TRANSLATIONS += \
    networkaccount_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image/Delete.qrc \
    json/jsonFile.qrc \
    qss.qrc

DISTFILES += \

unix:!macx: LIBS += -L$$[QT_INSTALL_LIBS]  -lkylinssoclient

INCLUDEPATH += $$[QT_INSTALL_LIBS]
DEPENDPATH += $$[QT_INSTALL_LIBS]
