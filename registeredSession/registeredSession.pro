QT += core dbus
QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-control-center-session
TEMPLATE = app

CONFIG += c++11 console link_pkgconfig
CONFIG -= app_bundle

DESTDIR = .
INCLUDEPATH += .

inst1.files += conf/com.control.center.qt.sessiondbus.service
inst1.path = /usr/share/dbus-1/services/
INSTALLS += inst1
#inst2.files += conf/com.control.center.qt.sessiondbus.conf
#inst2.path = /etc/dbus-1/session.d/
#INSTALLS += inst2

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
        json.cpp \
        main.cpp \
        sessiondbusregister.cpp

# Default rules for deployment.
target.path = /usr/bin/
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    json.h \
    sessiondbusregister.h
