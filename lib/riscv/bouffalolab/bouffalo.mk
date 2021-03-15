COMPONENT_LIB_ONLY := 1
COMPONENT_ADD_INCLUDEDIRS += $(BL60X_SDK_PATH)/../../../include
LIBS_PATH := $(BL60X_SDK_PATH)/../../lib/riscv/bouffalolab
LIBS ?= ejs
COMPONENT_ADD_LDFLAGS += -L$(LIBS_PATH) $(addprefix -l,$(LIBS))
ALL_LIB_FILES := $(patsubst %,$(LIBS_PATH)/lib%.a,$(LIBS))

$(warning $(ALL_LIB_FILES))

COMPONENT_ADD_LINKER_DEPS := $(ALL_LIB_FILES)
