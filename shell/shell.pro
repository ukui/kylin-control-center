#-------------------------------------------------
#
# Project created by QtCreator 2019-12-21T09:41:12
#
#-------------------------------------------------

QT       += core gui network x11extras svg xml dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-control-center
DESTDIR = ..
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../env.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'

LIBS += -L$$[QT_INSTALL_LIBS] -lX11 -lgsettings-qt

#LIBS += -L$$[QT_INSTALL_LIBS] -lkylinssoclient

##加载gio库和gio-unix库，用于处理desktop文件
CONFIG        += link_pkgconfig \
                 C++11
PKGCONFIG     += gio-2.0 \
                 gio-unix-2.0 \
                 libmatemixer \
                 gsettings-qt \

desktop.files += ukui-control-center.desktop
desktop.path = /usr/share/applications

schemes.files += $$PROJECT_ROOTDIR/data/*.xml
schemes.path = /usr/share/glib-2.0/schemas/

face.files += $$PROJECT_ROOTDIR/data/faces/
face.path = /usr/share/ukui/

mo.files += $$PROJECT_ROOTDIR/data/installer-timezones.mo
mo.path = /usr/share/locale/zh_CN/LC_MESSAGES/

target.source += $$TARGET
target.path = /usr/bin


INSTALLS +=  \
            target \
            desktop \
            schemes \
            face \
            mo

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
    pinyin.cpp \
    prescene.cpp \
    searchwidget.cpp \
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
    customstyle.h \
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
    res/secondaryleftmenu/About.svg \
    res/secondaryleftmenu/AboutWhite.svg \
    res/secondaryleftmenu/Area.svg \
    res/secondaryleftmenu/AreaWhite.svg \
    res/secondaryleftmenu/Audio.svg \
    res/secondaryleftmenu/AudioWhite.svg \
    res/secondaryleftmenu/Autoboot.svg \
    res/secondaryleftmenu/AutobootWhite.svg \
    res/secondaryleftmenu/Background.svg \
    res/secondaryleftmenu/BackgroundWhite.svg \
    res/secondaryleftmenu/Backup.svg \
    res/secondaryleftmenu/BackupWhite.svg \
    res/secondaryleftmenu/Bluetooth.svg \
    res/secondaryleftmenu/BluetoothWhite.svg \
    res/secondaryleftmenu/Cloud Account.svg \
    res/secondaryleftmenu/Cloud Account.svg \
    res/secondaryleftmenu/Cloud AccountWhite.svg \
    res/secondaryleftmenu/Cloud AccountWhite.svg \
    res/secondaryleftmenu/Dat.svg \
    res/secondaryleftmenu/DatWhite.svg \
    res/secondaryleftmenu/Defaultapp.svg \
    res/secondaryleftmenu/DefaultappWhite.svg \
    res/secondaryleftmenu/Desktop.svg \
    res/secondaryleftmenu/DesktopWhite.svg \
    res/secondaryleftmenu/Display.svg \
    res/secondaryleftmenu/DisplayWhite.svg \
    res/secondaryleftmenu/Experienceplan.svg \
    res/secondaryleftmenu/ExperienceplanWhite.svg \
    res/secondaryleftmenu/Fonts.svg \
    res/secondaryleftmenu/FontsWhite.svg \
    res/secondaryleftmenu/Keyboard.svg \
    res/secondaryleftmenu/KeyboardWhite.svg \
    res/secondaryleftmenu/Mouse.svg \
    res/secondaryleftmenu/MouseWhite.svg \
    res/secondaryleftmenu/Netconnect.svg \
    res/secondaryleftmenu/NetconnectWhite.svg \
    res/secondaryleftmenu/Notice.svg \
    res/secondaryleftmenu/NoticeWhite.svg \
    res/secondaryleftmenu/Power.svg \
    res/secondaryleftmenu/PowerWhite.svg \
    res/secondaryleftmenu/Printer.svg \
    res/secondaryleftmenu/PrinterWhite.svg \
    res/secondaryleftmenu/Proxy.svg \
    res/secondaryleftmenu/ProxyWhite.svg \
    res/secondaryleftmenu/Screenlock.svg \
    res/secondaryleftmenu/ScreenlockWhite.svg \
    res/secondaryleftmenu/Screensaver.svg \
    res/secondaryleftmenu/ScreensaverWhite.svg \
    res/secondaryleftmenu/SecurityCenter.svg \
    res/secondaryleftmenu/SecurityCenterWhite.svg \
    res/secondaryleftmenu/Shortcut.svg \
    res/secondaryleftmenu/ShortcutWhite.svg \
    res/secondaryleftmenu/Theme.svg \
    res/secondaryleftmenu/ThemeWhite.svg \
    res/secondaryleftmenu/TouchScreen.svg \
    res/secondaryleftmenu/TouchScreenWhite.svg \
    res/secondaryleftmenu/Touchpad.svg \
    res/secondaryleftmenu/TouchpadWhite.svg \
    res/secondaryleftmenu/Update.svg \
    res/secondaryleftmenu/UpdateWhite.svg \
    res/secondaryleftmenu/Userinfo.svg \
    res/secondaryleftmenu/UserinfoWhite.svg \
    res/secondaryleftmenu/Vino.svg \
    res/secondaryleftmenu/VinoWhite.svg \
    res/secondaryleftmenu/Vpn.svg \
    res/secondaryleftmenu/VpnWhite.svg
