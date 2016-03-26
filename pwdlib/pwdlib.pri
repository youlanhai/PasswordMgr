
HEADERS += \
	$$PWD/Pwd.h \
	$$PWD/pwdconfig.h \
	$$PWD/PwdMgr.h \
	$$PWD/PwdStream.h \
	$$PWD/util.h \
    $$PWD/pwdloader.h \
    $$PWD/pwdlog.h

SOURCES += \
	$$PWD/Pwd.cpp \
	$$PWD/PwdMgr.cpp \
	$$PWD/PwdStream.cpp \
	$$PWD/util.cpp \
    $$PWD/pwdloader.cpp \
    $$PWD/pwdlog.cpp

LIBS += -liconv

INCLUDEPATH += $$PWD/../third_party/openssl/mac/include
LIBS += $$PWD/../third_party/openssl/mac/lib/libcrypto.a $$PWD/../third_party/openssl/mac/lib/libssl.a
