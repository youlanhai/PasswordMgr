
CONFIG += c++11
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DESTDIR = $$PWD/bin

CONFIG(debug, debug|release){
    TARGET = pwdtool-debug
    DEFINES += DEBUG
}else{
    TARGET = pwdtool-release
    DEFINES += NDEBUG
}

include(pwdlib/pwdlib.pri)
include(pwdtool/pwdtool.pri)
