#-------------------------------------------------
#
# Project created by QtCreator 2019-12-21T09:41:12
#
#-------------------------------------------------

QT       += core gui network x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-control-center
DESTDIR = ..
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../env.pri)

#INCLUDEPATH   +=  \
#                 /usr/include/KF5/KWindowSystem/ \

LIBS += -L/usr/lib/ -lX11 -lKF5WindowSystem

#LIBS += -L$$[QT_INSTALL_LIBS] -lkylinssoclient

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 libmatemixer

inst1.files += ukui-control-center.desktop
inst1.path = /usr/share/applications
inst2.files += $$PROJECT_ROOTDIR/pluginlibs/
inst2.path = /usr/lib/control-center/
inst3.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.keybinding.gschema.xml
inst3.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.desktop.gschema.xml
inst3.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.notice.gschema.xml
inst3.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.experienceplan.gschema.xml
inst3.path = /usr/share/glib-2.0/schemas/
inst4.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.wifi.switch.gschema.xml
inst4.path = /usr/share/glib-2.0/schemas/
inst5.files += $$PROJECT_ROOTDIR/data/org.ukui.control-center.panel.plugins.gschema.xml
inst5.path = /usr/share/glib-2.0/schemas/
inst6.files += $$PROJECT_ROOTDIR/data/faces/
inst6.path = /usr/share/ukui/
inst7.files += $$PROJECT_ROOTDIR/data/installer-timezones.mo
inst7.path = /usr/share/locale/zh_CN/LC_MESSAGES/

target.source += $$TARGET
target.path = /usr/bin

INSTALLS +=  \
            inst1 \
            inst2 \
            inst3 \
            inst4 \
            inst5 \
            inst6 \
            inst7 \
            target \

INCLUDEPATH += qtsingleapplication
DEPENDPATH += qtsingleapplication

SOURCES += \
    customstyle.cpp \
        main.cpp \
        mainwindow.cpp \
    framelessExtended/cursorposcalculator.cpp \
    framelessExtended/framelesshandle.cpp \
    framelessExtended/widgethandlerealize.cpp \
    homepagewidget.cpp \
    modulepagewidget.cpp \
    utils/keyvalueconverter.cpp \
    component/leftwidgetitem.cpp \
    component/clicklabel.cpp \
    utils/functionselect.cpp \
    component/hoverwidget.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlocalpeer.cpp

HEADERS += \
    customstyle.h \
        mainwindow.h \
        interface.h \
    framelessExtended/cursorposcalculator.h \
    framelessExtended/framelesshandle.h \
    framelessExtended/framelesshandleprivate.h \
    framelessExtended/widgethandlerealize.h \
    homepagewidget.h \
    modulepagewidget.h \
    utils/keyvalueconverter.h \
    component/leftwidgetitem.h \
    component/clicklabel.h \
    utils/functionselect.h \
    component/hoverwidget.h \
    qtsingleapplication/qtsingleapplication_copy.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtlocalpeer.h

FORMS += \
        mainwindow.ui \
    homepagewidget.ui \
    modulepagewidget.ui

RESOURCES += \
    res/resfile.qrc

DISTFILES += \
    ../data/installer-timezones.mo \
    ../data/org.ukui.control-center.panel.plugins.gschema.xml \
    ../data/org.ukui.control-center.wifi.switch.gschema.xml
