TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/../../include
INCLUDEPATH += $$PWD/../../modules/eluaos/lualib
INCLUDEPATH += $$PWD/../../modules/eluaos/include
INCLUDEPATH += $$PWD/../../modules/eluaos/packages/lua-cjson
INCLUDEPATH += $$PWD/../../components/httpclient/inc
INCLUDEPATH += $$PWD/../../components/freertos/include
INCLUDEPATH += $$PWD/../../components/mbedtls/include
INCLUDEPATH += $$PWD/../../components/mbedtls/include/mbedtls

DEFINES += LUAT_CONF_DISABLE_ROTABLE
DEFINES += CONFIG_EVM_ECMA_TIMEOUT

unix {
    INCLUDEPATH += $$PWD/../../components/freertos/portable/ThirdParty/GCC/Posix
    INCLUDEPATH += $$PWD/../../components/freertos/portable/ThirdParty/GCC/Posix/utils
    LIBS += -lpthread
    LIBS += -L$$PWD/../../lib/x86/linux -lelua
}

win32 {
    INCLUDEPATH += $$PWD/../../components/freertos/portable/MSVC-MingW
    INCLUDEPATH += $$PWD/../../lib/x86/linux
    LIBS += -lwinmm
    LIBS += -L$$PWD/../../lib/x86/linux -lelua
}

include($$PWD/qt/lualib.pri)
include($$PWD/qt/luat.pri)
include($$PWD/qt/freertos.pri)
include($$PWD/qt/httpclient.pri)
include($$PWD/qt/mbedtls.pri)
include($$PWD/qt/port.pri)

SOURCES += \
        main.c \
    lnewstate.c

HEADERS += \
    include/luat_conf_bsp.h
