# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ../../include
COMPONENT_ADD_INCLUDEDIRS += ../../components/webclient/inc
COMPONENT_ADD_INCLUDEDIRS += ../../../ecma/inc
COMPONENT_ADD_INCLUDEDIRS += ../../../evm
COMPONENT_ADD_INCLUDEDIRS += ../../../evm/native/repl

COMPONENT_INCLUDES += bouffalolab

EVM_PATH := ../../../evm
ECMA_PATH := ../../../ecma

EVM_SOURCES := \
    $(EVM_PATH)/libevm/evm_compiler.c \
    $(EVM_PATH)/libevm/evm.c \
    $(EVM_PATH)/libevm/native.c \
    $(EVM_PATH)/libevm/evm_gc.c \
    $(EVM_PATH)/libevm/jsparser.c \
	$(EVM_PATH)/native/repl/evm_repl.c

ECMA_SOURCES := \
    $(ECMA_PATH)/src/ecma.c \
    $(ECMA_PATH)/src/ecma_array.c \
    $(ECMA_PATH)/src/ecma_array_prototype.c \
    $(ECMA_PATH)/src/ecma_boolean.c \
    $(ECMA_PATH)/src/ecma_builtin.c \
    $(ECMA_PATH)/src/ecma_console.c \
    $(ECMA_PATH)/src/ecma_function.c \
    $(ECMA_PATH)/src/ecma_function_prototype.c \
    $(ECMA_PATH)/src/ecma_math.c \
    $(ECMA_PATH)/src/ecma_number.c \
    $(ECMA_PATH)/src/ecma_object.c \
    $(ECMA_PATH)/src/ecma_object_prototype.c \
    $(ECMA_PATH)/src/ecma_regexp.c \
    $(ECMA_PATH)/src/ecma_string.c \
    $(ECMA_PATH)/src/ecma_timeout.c \
    $(ECMA_PATH)/src/re.c


## This component's src

COMPONENT_SRCS += $(EVM_SOURCES)
COMPONENT_SRCS += $(ECMA_SOURCES)
COMPONENT_SRCS += \
	common/evm_module.c \
	common/evm_module_assert.c \
	common/evm_module_process.c \
	common/evm_module_buffer.c \
	bouffalolab/evm_main.c \
	bouffalolab/evm_module_dns.c \
	bouffalolab/evm_module_net.c \
	bouffalolab/evm_module_timers.c \
	bouffalolab/evm_module_gpio.c \
	bouffalolab/evm_module_uart.c \
	bouffalolab/evm_module_fs.c \
	bouffalolab/evm_module_http.c \
	../../components/webclient/src/webclient.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS += common bouffalolab ../../../evm/libevm ../../../ecma/src ../../../evm/native/repl ../../components/webclient/src