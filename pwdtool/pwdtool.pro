
CONFIG += c++11
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

CONFIG(debug, debug|release){
	TARGET = pwdtool-debug
    DEFINES += DEBUG
}else{
	TARGET = pwdtool-release
    DEFINES += NDEBUG
}

include(../pwdlib/pwdlib.pri)

HEADERS += \
    mainwindow.h \
    document.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    document.cpp

FORMS += \
    mainwindow.ui
