TEMPLATE = app
TARGET = group-manager-server
INCLUDEPATH += .

QT += dbus core

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
DBUS_ADAPTORS += org.ukui.groupmanager.xml
DBUS_INTERFACES += org.ukui.groupmanager.xml

QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2 -O2

HEADERS +=  \
            custom_struct.h \
            group_manager_server.h
SOURCES += \
            group_manager_server.cpp \
            main.cpp

target.path = /usr/bin/
!isEmpty(target.path): INSTALLS += target

dbus_conf.path = /etc/dbus-1/system.d
dbus_conf.files +=  conf/org.ukui.groupmanager.conf
INSTALLS += dbus_conf

systemd_service.path = /lib/systemd/system
systemd_service.files += conf/ukui-group-manager.service
INSTALLS += systemd_service

polkit.path = /usr/share/polkit-1/actions/
polkit.files += conf/org.ukui.groupmanager.policy
INSTALLS += polkit







