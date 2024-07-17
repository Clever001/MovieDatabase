#-------------------------------------------------
#
# Project created by QtCreator 2024-06-27T17:10:26
#
#-------------------------------------------------

QT       += core gui
QMAKE_LFLAGS +=-static-libgcc -static-libstdc++

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MovieDatabase
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

CONFIG += c++11
CONFIG += static

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    MovieModel.cpp \
    startwindow.cpp \
    newmoviewindow.cpp \
    editmoviewindow.cpp \
    complexsearchwindow.cpp

HEADERS += \
        mainwindow.h \
    MovieModel.h \
    json.hpp \
    json_fwd.hpp \
    startwindow.h \
    newmoviewindow.h \
    editmoviewindow.h \
    complexsearchwindow.h

FORMS += \
        mainwindow.ui \
    startwindow.ui \
    newmoviewindow.ui \
    editmoviewindow.ui \
    complexsearchwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/project_images/manage_search.png \
    images/project_images/search.png \
    images/project_images/search_body.png \
    images/project_images/manage_search.svg \
    images/project_images/search_icon.svg

RESOURCES += \
    resource.qrc
