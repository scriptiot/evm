# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ../../include
COMPONENT_ADD_INCLUDEDIRS += ../include

COMPONENT_INCLUDES += bouffalolab

## This component's src
COMPONENT_SRCS += \
	lualib/bget.c \
	lualib/lbaselib.c \
	lualib/rotable.c \
	lualib/printf.c \
	lualib/linit.c

COMPONENT_SRCS += \
	bouffalolab/luat_timer_freertos.c \
	bouffalolab/luat_msgbus_freertos.c \
	bouffalolab/luat_malloc_bfl.c \
	bouffalolab/luat_uart_blf.c \
	bouffalolab/luat_bfl_base.c

COMPONENT_SRCS += \
	modules/luat_lib_rtos.c \
	modules/luat_lib_log.c \
	modules/luat_fs_weak.c \
	modules/luat_log_weak.c \
	modules/luat_malloc_weak.c \
	modules/luat_luat_bin.c \
	modules/luat_main.c 

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS += modules bouffalolab lualib