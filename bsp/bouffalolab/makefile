PROJECT_NAME := app
PROJECT_PATH := $(abspath .)
PROJECT_BOARD := evb
export PROJECT_PATH PROJECT_BOARD
#CONFIG_TOOLPREFIX :=

-include ./proj_config.mk

ifeq ($(origin BL60X_SDK_PATH), undefined)
BL60X_SDK_PATH_GUESS ?= $(shell pwd)
BL60X_SDK_PATH ?= $(BL60X_SDK_PATH_GUESS)/../../sdk/bl_iot_sdk
$(info ****** Please SET BL60X_SDK_PATH ******)
$(info ****** Trying SDK PATH [$(BL60X_SDK_PATH)])
endif

EXTRA_COMPONENT_DIRS += $(BL60X_SDK_PATH)/../../modules
EXTRA_COMPONENT_DIRS += $(BL60X_SDK_PATH)/../../lib/riscv/bouffalolab
COMPONENT_INCLUDES += $(BL60X_SDK_PATH)/../../modules/eluaos/bouffalolab

CFLAGS += -DFREERTOS

COMPONENTS_NETWORK := sntp dns_server
COMPONENTS_BLSYS   := bltime blfdt blmtd bloop loopadc looprt loopset
COMPONENTS_VFS     := romfs

INCLUDE_COMPONENTS += freertos_riscv_ram bl602 bl602_std bl602_wifi bl602_wifidrv hal_drv lwip lwip_dhcpd lwip_mdns vfs yloop utils netutils cli blog blog_testc
INCLUDE_COMPONENTS += easyflash4
INCLUDE_COMPONENTS += elua
INCLUDE_COMPONENTS += eluaos
INCLUDE_COMPONENTS += $(COMPONENTS_NETWORK)
INCLUDE_COMPONENTS += $(COMPONENTS_BLSYS)
INCLUDE_COMPONENTS += $(COMPONENTS_VFS)
INCLUDE_COMPONENTS += $(PROJECT_NAME)

include $(BL60X_SDK_PATH)/make_scripts_riscv/project.mk
