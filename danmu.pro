#-------------------------------------------------
#
# Project created by QtCreator 2018-02-24T19:25:31
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = danmu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    viewwidget.cpp \
    selectfile.cpp

HEADERS  += mainwindow.h \
    viewwidget.h \
    selectfile.h

FORMS    += mainwindow.ui \
    viewwidget.ui \
    selectfile.ui

DISTFILES += \
    danmu.pro.user

RESOURCES +=

RC_ICONS = image.ico
