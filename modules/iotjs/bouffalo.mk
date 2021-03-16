# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ../../include
COMPONENT_ADD_INCLUDEDIRS += ../../components/webclient/inc

COMPONENT_INCLUDES += bouffalolab

## This component's src

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

COMPONENT_SRCDIRS += common bouffalolab ../../components/webclient/src
