TEMPLATE = app
TARGET = ejs
VERSION = 1.0
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(release, release|debug) {
    QMAKE_CFLAGS += -O2 -flto  -ffunction-sections -fdata-sections  -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--strip-all
    QMAKE_LFLAGS += -Wl,--gc-sections -Wl,--strip-all
} else {
    QMAKE_CFLAGS += -ffunction-sections -fdata-sections
    QMAKE_LFLAGS += -Wl,--gc-sections
}

DEFINES += QMAKE_TARGET=\\\"$$TARGET\\\" QMAKE_VERSION=\\\"$$VERSION\\\"
DEFINES += EVM_LANG_ENABLE_JAVASCRIPT
DEFINES += EVM_LANG_ENABLE_JSON
DEFINES += EVM_LANG_ENABLE_XML

unix {
    exists($$PWD/../lib/x86_linux/libejs.a) {
        LIBS += $$PWD/../lib/x86_linux/libejs.a
    }
    exists($$OUT_PWD/../libejs/libejs.a) {
        LIBS += $$OUT_PWD/../libejs/libejs.a
    }
}

win32{
    exists($$PWD/../lib/x86_win64/libejs.a) {
        LIBS += $$PWD/../lib/x86_win64/libejs.a
    }
    exists($$OUT_PWD/../libejs/libejs.a) {
        LIBS += $$OUT_PWD/../libejs/libejs.a
    }
}

INCLUDEPATH += $$PWD/../libevm
INCLUDEPATH += $$PWD/../lib/include
INCLUDEPATH += $$PWD/../thirds/cjson

include($$PWD/../thirds/cjson/cjson.pri)


exists($$PWD/../main.c) {
    SOURCES += \
        ../main.c
}

exists($$PWD/main.c) {
    SOURCES += \
        main.c
}
