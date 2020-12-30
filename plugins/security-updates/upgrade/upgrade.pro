include(../../../env.pri)

QT       +=  core gui widgets network dbus QApt sql

TEMPLATE = lib
CONFIG += plugin link_pkgconfig c++11
PKGCONFIG += gsettings-qt

TARGET = $$qtLibraryTarget(upgrade)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}


# 应用图标装载
#icon.path = /usr/share/pixmaps
#icon.files = img/kylin-update-manager.png

# desktop文件装载
#desktop.path = /usr/share/applications/
#desktop.files = kylin-update-manager.desktop

# gsetting文件装载
#schemes.files = data/org.kylin-update-manager-data.gschema.xml
#schemes.path = /usr/share/glib-2.0/schemas/

INCLUDEPATH   +=  \
                 $$PROJECT_ROOTDIR \

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/appupdate.cpp \
    src/changelogwidget.cpp \
    src/daemonipcdbus.cpp \
    src/listwig.cpp \
    src/logwindow.cpp \
    src/m_updatelog.cpp \
    src/mainwindow.cpp \
    src/shadowwidget.cpp \
    src/switchbutton.cpp \
    src/tabwidget.cpp \
    src/traybusthread.cpp \
    src/updatedbus.cpp \
    src/updatelog.cpp \
    src/widgetstyle.cpp \
    src/updatesource.cpp \
    upgrade.cpp

HEADERS += \
    src/appupdate.h \
    src/changelogwidget.h \
    src/connection.h \
    src/daemonipcdbus.h \
    src/listwig.h \
    src/logwindow.h \
    src/m_updatelog.h \
    src/mainwindow.h \
    src/metatypes.h \
    src/shadowwidget.h \
    src/switchbutton.h \
    src/tabwidget.h \
    src/traybusthread.h \
    src/updatedbus.h \
    src/updatelog.h \
    src/utils.h \
    src/widgetstyle.h \
    src/updatesource.h \
    upgrade.h

# Default rules for deployment.
INSTALLS += target  \
            icon    \
            desktop \
            schemes


