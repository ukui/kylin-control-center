QT       += core dbus
QT       -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launchSysDbus
TEMPLATE = app

CONFIG += console c++11 link_pkgconfig
CONFIG -= app_bundle

DESTDIR = .
INCLUDEPATH += .

inst1.files += conf/com.control.center.qt.systemdbus.service
inst1.path = /usr/share/dbus-1/system-services/
inst2.files += conf/com.control.center.qt.systemdbus.conf
inst2.path = /etc/dbus-1/system.d/

target.source += $$TARGET
target.path = /usr/bin
INSTALLS += \
    target \
    inst1 \
    inst2 \

HEADERS += \
    sysdbusregister.h

SOURCES += \
    main.cpp \
    sysdbusregister.cpp
