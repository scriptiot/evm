TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += EVM_LANG_ENABLE_REPL
#DEFINES += CONFIG_EVM_MODULE_FS
DEFINES += CONFIG_EVM_MODULE_NET
#DEFINES += CONFIG_EVM_MODULE_PROCESS
#DEFINES += CONFIG_EVM_MODULE_EVENTS
#DEFINES += CONFIG_EVM_MODULE_DNS
#DEFINES += CONFIG_EVM_MODULE_TIMERS
#DEFINES += CONFIG_EVM_MODULE_BUFFER
#DEFINES += CONFIG_EVM_MODULE_ASSERT

LIBS += -L$$PWD/../../../lib/x86/linux -lecma -lejs
LIBS += -lpthread
LIBS += -lrt
INCLUDEPATH += $$PWD/../../../include

SOURCES += \
        main.c \
    ../../../modules/iotjs/linux/evm_main.c \
    ../../../modules/iotjs/linux/evm_module_net.c \
    ../../../modules/iotjs/common/evm_module_process.c \
    ../../../modules/iotjs/common/evm_module.c \
    ../../../modules/iotjs/linux/evm_module_fs.c \
    ../../../modules/iotjs/common/evm_module_events.c \
    ../../../modules/iotjs/linux/evm_module_dns.c \
    ../../../modules/iotjs/linux/evm_module_timers.c \
    ../../../modules/iotjs/common/evm_module_buffer.c \
    ../../../modules/iotjs/common/evm_module_assert.c
