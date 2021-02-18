TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += EVM_LANG_ENABLE_REPL
DEFINES += CONFIG_EVM_MODULE_FS
DEFINES += CONFIG_EVM_MODULE_NET
DEFINES += CONFIG_EVM_MODULE_PROCESS
DEFINES += CONFIG_EVM_MODULE_EVENTS

LIBS += -L$$PWD/../../../lib/x86/linux -lecma -lejs
LIBS += -lpthread
INCLUDEPATH += $$PWD/../../../include

SOURCES += \
        main.c \
    ../../../modules/linux/evm_main.c \
    ../../../modules/linux/evm_module_net.c \
    ../../../modules/linux/evm_module_process.c \
    ../../../modules/linux/evm_module.c \
    ../../../modules/linux/evm_module_fs.c \
    ../../../modules/linux/evm_module_events.c
