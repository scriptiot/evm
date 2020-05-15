# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(ejs)

set(EVM_BASE ${ZEPHYR_BASE}/../../../)
# set(LIBEJS ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libejs.a)

if(EXISTS ${LIBEJS})
    target_link_libraries(app PRIVATE ${LIBEJS})
else()
    set(EVM_DIR ${EVM_BASE}/../evm/)
    set(LIBEVM_DIR ${EVM_DIR}/libevm/)
    set(LIBEVM_PRIVATE_DIR ${EVM_DIR}/private/)
    set(EVM_NATIVE_DIR ${EVM_DIR}/native/)
    set(EVM_REPL_DIR ${EVM_NATIVE_DIR}/repl/)
    set(EVM_QML_DIR ${EVM_NATIVE_DIR}/qml/)
    set(EVM_PYTHON_DIR ${EVM_NATIVE_DIR}/python/)
    set(CONFIG_EVM_DIR ${EVM_NATIVE_DIR}/driver/)

    set(evm_common_sources 
        ${LIBEVM_DIR}/compiler.c
        ${LIBEVM_DIR}/evm.c
        ${LIBEVM_DIR}/nevm.c
        ${LIBEVM_DIR}/gc.c
        ${LIBEVM_DIR}/native.c
        ${LIBEVM_DIR}/executable.c
        ${LIBEVM_DIR}/jsonparser.c
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

    set(libejs_sources 
        ${evm_common_sources}
        ${LIBEVM_DIR}/jsparser.c
        ${evm_repl_sources}
    )

    set(libqml_sources 
        ${evm_common_sources}
        ${LIBEVM_DIR}/jsparser.c
        ${LIBEVM_DIR}/qmlparser.c
        ${evm_qml_sources}
        ${evm_repl_sources}
    )

    set(libepy_sources 
        ${evm_common_sources}
        ${LIBEVM_DIR}/pyparser.c
        ${evm_repl_sources}
        ${CONFIG_EVM_sources}
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
    add_dependencies(ejs zephyr_generated_headers)
    target_link_libraries(app PRIVATE ejs)
endif()

set(evm_ecma_sources 
    ${EVM_BASE}/modules/ecma/ecma.c
    ${EVM_BASE}/modules/ecma/ecma_object.c
    ${EVM_BASE}/modules/ecma/ecma_array.c
    ${EVM_BASE}/modules/ecma/ecma_math.c
    ${EVM_BASE}/modules/ecma/ecma_regexp.c
    ${EVM_BASE}/modules/ecma/ecma_number.c
    ${EVM_BASE}/modules/ecma/ecma_string.c
    ${EVM_BASE}/modules/ecma/re.c
)

set(evm_driver_sources 
    ${EVM_BASE}/drivers/zephyr/adc_driver.c 
    ${EVM_BASE}/drivers/zephyr/can_driver.c 
    ${EVM_BASE}/drivers/zephyr/dac_driver.c
    ${EVM_BASE}/drivers/zephyr/flash_driver.c
    ${EVM_BASE}/drivers/zephyr/gpio_driver.c
    ${EVM_BASE}/drivers/zephyr/i2c_driver.c
    ${EVM_BASE}/drivers/zephyr/rtc_driver.c
    ${EVM_BASE}/drivers/zephyr/spi_driver.c
    ${EVM_BASE}/drivers/zephyr/timer_driver.c
    ${EVM_BASE}/drivers/zephyr/uart_driver.c
    ${EVM_BASE}/drivers/zephyr/watchdog_driver.c
    ${EVM_BASE}/drivers/zephyr/pwm_driver.c
    ${EVM_BASE}/drivers/zephyr/sys_driver.c
)

set(evm_modules_sources 
    ${EVM_BASE}/modules/evm/evm_module.c 
    ${EVM_BASE}/modules/evm/evm_module_adc.c 
    ${EVM_BASE}/modules/evm/evm_module_can.c
    ${EVM_BASE}/modules/evm/evm_module_dac.c
    ${EVM_BASE}/modules/evm/evm_module_flash.c
    ${EVM_BASE}/modules/evm/evm_module_i2c.c
    ${EVM_BASE}/modules/evm/evm_module_lcd.c
    ${EVM_BASE}/modules/evm/evm_module_led.c
    ${EVM_BASE}/modules/evm/evm_module_pin.c
    ${EVM_BASE}/modules/evm/evm_module_rtc.c
    ${EVM_BASE}/modules/evm/evm_module_spi.c
    ${EVM_BASE}/modules/evm/evm_module_switch.c
    ${EVM_BASE}/modules/evm/evm_module_timer.c
    ${EVM_BASE}/modules/evm/evm_module_uart.c
    ${EVM_BASE}/modules/evm/evm_module_watchdog.c
    ${EVM_BASE}/modules/evm/evm_module_led.c
)

set(libevm_module_sources 
    ${evm_driver_sources}
    ${evm_modules_sources}
    ${evm_ecma_sources}
)

add_definitions("-DQMAKE_TARGET=\"ejs\"")
add_definitions("-DQMAKE_VERSION=\"1.0\"")
remove_definitions("-DWall")

include_directories(app PRIVATE ${EVM_BASE}/include)
include_directories(app PRIVATE ${EVM_BASE}/ejs)
include_directories(app PRIVATE ${EVM_BASE}/modules/ecma)
include_directories(app PRIVATE src)

target_sources(app PRIVATE ${libevm_module_sources})
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE ${EVM_BASE}/ejs/evm_main.c)
target_sources(app PRIVATE ${EVM_BASE}/ejs/uol_output.c)

# ecma module config
add_definitions("-DCONFIG_EVM_ECMA_OBJECT")
add_definitions("-DCONFIG_EVM_ECMA_ARRAY")
add_definitions("-DCONFIG_EVM_ECMA_REGEXP")
add_definitions("-DCONFIG_EVM_ECMA_MATH")
add_definitions("-DCONFIG_EVM_ECMA_NUMBER")
add_definitions("-DCONFIG_EVM_ECMA_STRING")
