EVM_DIR = ../../../../evm
ECMA_DIR = ../../../../ecma

#check evm ecma directory is exists
#if not exists, abort
is_evm_exists = $(shell if [ -d $(EVM_DIR) ]; then echo 0; else echo 1; fi)
ifeq ($(is_evm_exists), 1)
	#if evm directory not exists, exit
	@echo -e $(is_evm_exists)
endif

is_ecma_exists = $(shell if [ -d $(ECMA_DIR) ]; then echo 0; else echo 1; fi)
ifeq ($(is_ecma_exists), 1)
	#if ecma directory not exists, exit
	@echo -e $(is_ecma_exists)
endif

#check riscv library directory is exists
#if not exists, create
LIB_DIRS = .
# is_lib_exists = $(shell if [ -d $(LIB_DIRS) ]; then echo "exist"; else echo "noexist"; fi)
# ifeq ("$(is_lib_exists)", "noexist")
# 	$(shell mkdir -p $(LIB_DIRS))
# endif

BL60X_SDK_PATH = ../../../sdk/bl_iot_sdk
CONFIG_TOOLPREFIX ?= $(BL60X_SDK_PATH)/toolchain/riscv/$(shell uname |cut -d '_' -f1)/bin/riscv64-unknown-elf-

CC := $(call dequote,$(CONFIG_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_TOOLPREFIX))g++
LD := $(call dequote,$(CONFIG_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_TOOLPREFIX))objcopy
SIZE := $(call dequote,$(CONFIG_TOOLPREFIX))size
STRIP := $(call dequote,$(CONFIG_TOOLPREFIX))strip

LIB         =$(AR) -rc
DEFINES     = -DEVM_LANG_ENABLE_JAVASCRIPT \
			-DEVM_LANG_ENABLE_REPL \
			-DCONFIG_EVM_ECMA_OBJECT \
			-DCONFIG_EVM_ECMA_ARRAY \
			-DCONFIG_EVM_ECMA_NUMBER \
			-DCONFIG_EVM_ECMA_STRING \
			-DCONFIG_EVM_ECMA_MATH \
			-DCONFIG_EVM_ECMA_TIMEOUT \
			-DCONFIG_EVM_ECMA_CONSOLE

CFLAGS      = -pipe -ffunction-sections -fdata-sections -g -Wall -W -fPIC -Wl,--strip-all $(DEFINES)
INCLUDES  	+=-I$(EVM_PATH) \
			  -I$(ECMA_DIR)/inc \
			  -I$(EVM_PATH)/native/repl

LIBOBJ		=$(patsubst %.c, %.o, $(SOURCES_LIBEJS))
OBJ			=$(patsubst %.c, %.o, $(SOURCES))
TARGET_LIB  =libejs.a
TARGET		=ejs
DEST_DIR    =$(LIB_DIRS)

SOURCES_LIBEJS = \
		$(EVM_PATH)/libevm/evm_compiler.c \
		$(EVM_PATH)/libevm/evm.c \
		$(EVM_PATH)/libevm/native.c \
		$(EVM_PATH)/libevm/evm_gc.c \
		$(EVM_PATH)/libevm/jsparser.c \
		$(EVM_PATH)/native/repl/evm_repl.c

SOURCES = \
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

$(TARGET_LIB): $(LIBOBJ)
	@mkdir -p $(DEST_DIR)
	$(LIB)  $(DEST_DIR)/$(TARGET_LIB) $(LIBOBJ)
	@rm -rf $(LIBOBJ)

#links
$(TARGET): $(TARGET_LIB) $(OBJ) 
	@mkdir -p $(DEST_DIR)
	$(CC) $(OBJ)  -L$(DEST_DIR) -l$(TARGET) -o $(DEST_DIR)/$(TARGET)
	@rm -rf $(OBJ)

#compile
%.o: %.c
	$(CC) $(INCLUDES) -c $(CFLAGS) $< -o $@


clean:
	@echo $(LIB_PATH)
	@echo "Remove linked and compiled files......"
	rm -rf $(OBJ) $(LIBOBJ) $(TARGET_LIB) $(TARGET) $(DEST_DIR) 

.PHONY:clean ejs