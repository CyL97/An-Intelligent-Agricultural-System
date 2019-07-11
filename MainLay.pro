#-------------------------------------------------
#
# Project created by QtCreator 2019-06-26T20:25:49
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += serialport
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MainLay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    keyBoard.cpp \
    mythread.cpp \
    wthread.cpp

HEADERS  += mainwindow.h \
    keyBoard.h \
    mythread.h \
    wthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    picture.qrc

CONFIG+=thread
