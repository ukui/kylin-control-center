QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras quickwidgets quick



LIBS          += -L$$[QT_INSTALL_LIBS] -lgsettings-qt -lX11 -lxkbcommon -lxkbfile
CONFIG += c++11


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
    application.cpp \
    doodad.cpp \
    geometry.cpp \
    key.cpp \
    main.cpp \
    outline.cpp \
    row.cpp \
    section.cpp \
    shape.cpp \
    xkbobject.cpp

HEADERS += \
    application.h \
    config-workspace.h \
    doodad.h \
    geometry.h \
    key.h \
    outline.h \
    row.h \
    section.h \
    shape.h \
    xkbobject.h

FORMS +=

target.source += $$TARGET
target.path = /usr/bin

INSTALLS += target

RESOURCES += \
    qml.qrc

