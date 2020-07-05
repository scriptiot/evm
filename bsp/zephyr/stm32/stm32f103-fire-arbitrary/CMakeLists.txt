# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# set board name
set(BOARD stm32f103_fire_arbitrary)

# board, dts, soc
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(DTS_ROOT ${CMAKE_CURRENT_LIST_DIR}/dts)

# evm path
set(EVM_BASE ${ZEPHYR_BASE}/../../../)

include_directories(app PRIVATE src)

# heatshrink module
list(APPEND ZEPHYR_EXTRA_MODULES
    ${EVM_BASE}/modules/heatshrink
)

# evm module
list(APPEND ZEPHYR_EXTRA_MODULES
    ${EVM_BASE}/modules/evm
)

# ecma
list(APPEND ZEPHYR_EXTRA_MODULES
    ${EVM_BASE}/modules/ecma
)

# driver
list(APPEND ZEPHYR_EXTRA_MODULES
    ${EVM_BASE}/drivers
)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(evm)

if(CONFIG_EVM_LANG_JAVASCRIPT)
    set(LIBEVM ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libejs.a)
elseif(CONFIG_EVM_LANG_PYTHON)
    set(LIBEVM ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libepy.a)
elseif(CONFIG_EVM_LANG_QML)
    set(LIBEVM ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libeqml.a)   
else()
    set(LIBEVM ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libejs.a)
endif()

message(${LIBEVM})
target_link_libraries(app PRIVATE ${LIBEVM})

include_directories(app PRIVATE ${EVM_BASE}/include)
include_directories(app PRIVATE ${EVM_BASE}/bsp/common)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE ${EVM_BASE}/bsp/common/evm_main.c)
target_sources(app PRIVATE src/evm_board.c)
target_sources(app PRIVATE ${EVM_BASE}/bsp/common/uol_output.c)
