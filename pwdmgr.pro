
CONFIG += c++11
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DESTDIR = $$PWD/bin

CONFIG(debug, debug|release){
    TARGET = pwdtool-debug
    DEFINES += DEBUG
}else{
    TARGET = pwdtool
    DEFINES += NDEBUG
}

include(pwdlib/pwdlib.pri)
include(pwdtool/pwdtool.pri)

INCLUDEPATH += $$PWD/third_party/openssl/mac/include
LIBS += $$PWD/third_party/openssl/mac/lib/libcrypto.a $$PWD/third_party/openssl/mac/lib/libssl.a

mac{
ICON += pwdtool/res/icon.icns
}

win32{
RC_FILE += pwdtool/res/pwdtool.rc
}
