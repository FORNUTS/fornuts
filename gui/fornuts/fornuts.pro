#-------------------------------------------------
#
# Project created by QtCreator 2013-03-20T12:25:57
#
#-------------------------------------------------
CONFIG += static
QMAKE_LFLAGS+=-static-libgcc

QT       += core gui
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fornuts
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../core/iniparser.c \
    ../../core/forcore.cpp \
    ../../core/dictionary.c \
    qcustomplot.cpp \
    aboutwindow.cpp \
    resultswindow.cpp

HEADERS  += mainwindow.h \
    ../../core/iniparser.h \
    ../../core/dictionary.h \
    ../../core/forcore.h \
    qcustomplot.h \
    aboutwindow.h \
    resultswindow.h

FORMS    += mainwindow.ui \
    aboutwindow.ui \
    resultswindow.ui

RESOURCES += \
    fonts.qrc \
    images.qrc

RC_FILE = icons.rc
