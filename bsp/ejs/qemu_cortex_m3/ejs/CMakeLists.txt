# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(eqml)

set(EVM_BASE ${ZEPHYR_BASE}/../../../)


if (EXISTS ${EVM_DEV})
    message(${EVM_DEV})
    set(CMAKE_MODULE_PATH ${EVM_DEV})
    find_package(ejs)
    target_link_libraries(app PRIVATE eqml)
else()
    set(LIBEJS ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libeqml.a)
    message(${LIBEJS})
    target_link_libraries(app PRIVATE ${LIBEJS})
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
    ${EVM_BASE}/drivers/zephyr/uol/adc_driver.c 
    ${EVM_BASE}/drivers/zephyr/uol/can_driver.c 
    ${EVM_BASE}/drivers/zephyr/uol/dac_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/flash_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/gpio_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/i2c_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/rtc_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/spi_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/timer_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/uart_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/watchdog_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/pwm_driver.c
    ${EVM_BASE}/drivers/zephyr/uol/sys_driver.c
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
