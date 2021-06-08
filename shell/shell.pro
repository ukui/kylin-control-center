#-------------------------------------------------
#
# Project created by QtCreator 2019-12-21T09:41:12
#
#-------------------------------------------------

QT       += core gui network x11extras svg xml dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quickwidgets

TARGET = ukui-control-center
DESTDIR = ..
TEMPLATE = app

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../env.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'


LIBS += -L$$[QT_INSTALL_LIBS] -lX11 -lgsettings-qt -lXi
LIBS += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lupower-glib

#LIBS += -L$$[QT_INSTALL_LIBS] -lkylinssoclient

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 gsettings-qt \
                 libmatemixer

desktop.files += ukui-control-center.desktop
desktop.path = /usr/share/applications

schemes.files += $$PROJECT_ROOTDIR/data/*.xml
schemes.path = /usr/share/glib-2.0/schemas/

face.files += $$PROJECT_ROOTDIR/data/faces/
face.path = /usr/share/ukui/

mo.files += $$PROJECT_ROOTDIR/data/installer-timezones.mo
mo.path = /usr/share/locale/zh_CN/LC_MESSAGES/

contains(QMAKE_HOST.arch, sw_64){
    guideCN.files += $$PROJECT_ROOTDIR/data/zh_CN_SW/zh_CN/
    guideCN.path += /usr/share/kylin-user-guide/data/guide/ukui-control-center/
}else{
    guideCN.files += $$PROJECT_ROOTDIR/data/zh_CN/
    guideCN.path += /usr/share/kylin-user-guide/data/guide/ukui-control-center/
}

guideEN.files += $$PROJECT_ROOTDIR/data/en_US/
guideEN.path += /usr/share/kylin-user-guide/data/guide/ukui-control-center/

imageformats.files += $$PROJECT_ROOTDIR/data/ukui-control-center.png
imageformats.path = /usr/share/kylin-user-guide/data/guide/ukui-control-center/

target.source += $$TARGET
target.path = /usr/bin


INSTALLS +=  \
            target  \
            desktop \
            schemes \
            face    \
            mo      \
            guideCN \
            imageformats \
            guideEN  \

INCLUDEPATH += qtsingleapplication
DEPENDPATH += qtsingleapplication

SOURCES += \
    component/leftmenulist.cpp \
    customstyle.cpp \
    main.cpp \
    mainwindow.cpp \
    framelessExtended/cursorposcalculator.cpp \
    framelessExtended/framelesshandle.cpp \
    framelessExtended/widgethandlerealize.cpp \
    homepagewidget.cpp \
    modulepagewidget.cpp \
    pinyin.cpp \
    prescene.cpp \
    searchwidget.cpp \
    ukccabout.cpp \
    utils/devicesmonitor.cpp \
    utils/keyvalueconverter.cpp \
    component/leftwidgetitem.cpp \
    component/clicklabel.cpp \
    utils/functionselect.cpp \
    component/hoverwidget.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    utils/utils.cpp \
    utils/xatom-helper.cpp

HEADERS += \
    component/leftmenulist.h \
    customstyle.h \
    devicesmonitor.h \
    mainwindow.h \
    interface.h \
    framelessExtended/cursorposcalculator.h \
    framelessExtended/framelesshandle.h \
    framelessExtended/framelesshandleprivate.h \
    framelessExtended/widgethandlerealize.h \
    homepagewidget.h \
    modulepagewidget.h \
    pinyin.h \
    prescene.h \
    searchwidget.h \
    ukccabout.h \
    utils/keyvalueconverter.h \
    component/leftwidgetitem.h \
    component/clicklabel.h \
    utils/functionselect.h \
    component/hoverwidget.h \
    qtsingleapplication/qtsingleapplication_copy.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtlocalpeer.h \
    utils/utils.h \
    utils/xatom-helper.h

FORMS += \
    mainwindow.ui \
    homepagewidget.ui \
    modulepagewidget.ui

RESOURCES += \
    res/resfile.qrc

DISTFILES += \
    ../data/installer-timezones.mo \
    ../data/org.ukui.control-center.panel.plugins.gschema.xml \
    ../data/org.ukui.control-center.personalise.gschema.xml \
    ../data/org.ukui.control-center.wifi.switch.gschema.xml \
    ../data/org.ukui.control-center.vino.xml \
    res/search.xml
