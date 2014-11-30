#-------------------------------------------------
#
# Project created by QtCreator 2014-11-25T21:38:55
#
#-------------------------------------------------

QT       += core gui
QT			 += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tips
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbadddialog.cpp

HEADERS  += mainwindow.h \
    dbadddialog.h

FORMS    += mainwindow.ui \
    dbadddialog.ui
