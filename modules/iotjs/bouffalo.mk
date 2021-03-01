# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ../../include

## This component's src
COMPONENT_SRCS := common/evm_module_assert.c \
				bouffalolab/evm_module_socket.c
					

COMPONENT_SRCDIRS += common bouffalolab