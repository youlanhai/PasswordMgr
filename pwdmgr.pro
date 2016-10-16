
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

macx{
ICON += pwdtool/res/icon.icns

INCLUDEPATH += $$PWD/third_party/openssl/mac/include
LIBS += -L$$PWD/third_party/openssl/mac/lib -lcrypto -lssl
LIBS += -liconv

}else:win32{

RC_FILE += pwdtool/res/pwdtool.rc

INCLUDEPATH += $$PWD/third_party/openssl/win32/include
LIBS += -L$$PWD/third_party/openssl/win32/lib -llibeay32 -lssleay32

}
