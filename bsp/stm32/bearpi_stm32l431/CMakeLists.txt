# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(BOARD bearpi_stm32l431)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(ejs)

set(EVM_DIR ${ZEPHYR_BASE}/../../../evm/)
set(LIBEVM_DIR ${EVM_DIR}/libevm/)
set(LIBEVM_PRIVATE_DIR ${EVM_DIR}/private/)
set(EVM_NATIVE_DIR ${EVM_DIR}/native/)
set(EVM_ECMA_DIR ${EVM_NATIVE_DIR}/ecma/)
set(EVM_REPL_DIR ${EVM_NATIVE_DIR}/repl/)
set(EVM_QML_DIR ${EVM_NATIVE_DIR}/qml/)
set(EVM_PYTHON_DIR ${EVM_NATIVE_DIR}/python/)
set(EVM_DRIVER_DIR ${EVM_NATIVE_DIR}/driver/)

set(ejs_sources
    ${ZEPHYR_BASE}/../../ejs/evm_main.c 
)

set(ejs_common_sources 
    ${LIBEVM_DIR}/compiler.c
    ${LIBEVM_DIR}/evm.c
    ${LIBEVM_DIR}/nevm.c
    ${LIBEVM_DIR}/gc.c
    ${LIBEVM_DIR}/native.c
    ${LIBEVM_DIR}/executable.c
    ${LIBEVM_DIR}/jsonparser.c
)

set(evm_driver_sources 
    ${ZEPHYR_BASE}/../../drivers/zephyr/adc_driver.c 
    ${ZEPHYR_BASE}/../../drivers/zephyr/can_driver.c 
    ${ZEPHYR_BASE}/../../drivers/zephyr/dac_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/gpio_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/i2c_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/rtc_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/spi_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/timer_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/uart_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/wdg_driver.c
    ${ZEPHYR_BASE}/../../drivers/zephyr/pwm_driver.c
)

set(evm_modules 
    ${ZEPHYR_BASE}/../../components/evm/evm_module.c 
    ${ZEPHYR_BASE}/../../components/evm/evm_module_adc.c 
    ${ZEPHYR_BASE}/../../components/evm/evm_module_can.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_dac.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_flash.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_i2c.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_lcd.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_led.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_pin.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_rtc.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_spi.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_switch.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_timer.c
    ${ZEPHYR_BASE}/../../components/evm/evm_module_uart.c
)

set(evm_repl_sources 
    ${EVM_REPL_DIR}/evm_repl.c 
    ${EVM_REPL_DIR}/rbuff.c 
    ${EVM_REPL_DIR}/evm_repl_history.c
)

set(evm_qml_sources 
    ${EVM_QML_DIR}/qml_module.c
)

set(evm_python_sources 
    ${EVM_PYTHON_DIR}/python_builtins.c
)

set(evm_ecma_sources 
    ${EVM_ECMA_DIR}/ecma.c
    ${EVM_ECMA_DIR}/ecma_object.c
    ${EVM_ECMA_DIR}/ecma_array.c
    ${EVM_ECMA_DIR}/ecma_math.c
    ${EVM_ECMA_DIR}/ecma_regexp.c
    ${EVM_ECMA_DIR}/ecma_number.c
    ${EVM_ECMA_DIR}/ecma_date.c
    ${EVM_ECMA_DIR}/ecma_string.c
    ${EVM_ECMA_DIR}/ecma_date.c
    ${EVM_ECMA_DIR}/re.c
)

set(libejs_sources 
    ${ejs_sources}
    ${ejs_common_sources}
    ${LIBEVM_DIR}/jsparser.c
    ${evm_repl_sources}
    ${evm_driver_sources}
    ${evm_modules}
)

set(libqml_sources 
    ${ejs_common_sources}
    ${LIBEVM_DIR}/jsparser.c
    ${LIBEVM_DIR}/qmlparser.c
    ${evm_ecma_sources}
    ${evm_qml_sources}
    ${evm_repl_sources}
)

set(libepy_sources 
    ${ejs_common_sources}
    ${LIBEVM_DIR}/pyparser.c
    ${evm_repl_sources}
    ${evm_driver_sources}
)

add_definitions("-DEVM_LANG_ENABLE_JAVASCRIPT")
add_definitions("-DEVM_LANG_ENABLE_REPL")
add_definitions("-DEVM_LANG_ENABLE_EXECUTABLE")
add_definitions("-DEVM_LANG_ENABLE_UOL")

zephyr_library_named(ejs)
foreach(i ${libejs_sources})
    zephyr_library_sources(${i})
endforeach(i)

zephyr_library_include_directories(
    ${LIBEVM_DIR}
    ${LIBEVM_PRIVATE_DIR}
)

add_definitions("-DQMAKE_TARGET=\"ejs\"")
add_definitions("-DQMAKE_VERSION=\"1.0\"")
remove_definitions("-DWall")

include_directories(app PRIVATE ${ZEPHYR_BASE}/../../include)
include_directories(app PRIVATE ${ZEPHYR_BASE}/../../ejs)

target_sources(app PRIVATE src/main.c)
target_link_libraries(app PRIVATE ejs)

add_definitions("-DEVM_DRIVER_GPIO")
