

QT += core gui widgets
CONFIG += c++11

TEMPLATE = app
DESTDIR = $$PWD/bin

CONFIG(debug, debug|release){
	TARGET = pwdmgr-debug
}else{
	TARGET = pwdmgr-release
}

include(pwdlib/pwdlib.pri)
include(pwdmgr/pwdmgr.pri)
