# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(ejs)

set(EVM_BASE ${ZEPHYR_BASE}/../../../)
set(LIBEJS ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libejs.a)

if(EXISTS ${LIBEJS})
    target_link_libraries(app PRIVATE ${LIBEJS})
endif()

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
    ${EVM_BASE}/components/evm/evm_module.c 
    ${EVM_BASE}/components/evm/evm_module_adc.c 
    ${EVM_BASE}/components/evm/evm_module_can.c
    ${EVM_BASE}/components/evm/evm_module_dac.c
    ${EVM_BASE}/components/evm/evm_module_flash.c
    ${EVM_BASE}/components/evm/evm_module_i2c.c
    ${EVM_BASE}/components/evm/evm_module_lcd.c
    ${EVM_BASE}/components/evm/evm_module_led.c
    ${EVM_BASE}/components/evm/evm_module_pin.c
    ${EVM_BASE}/components/evm/evm_module_rtc.c
    ${EVM_BASE}/components/evm/evm_module_spi.c
    ${EVM_BASE}/components/evm/evm_module_switch.c
    ${EVM_BASE}/components/evm/evm_module_timer.c
    ${EVM_BASE}/components/evm/evm_module_uart.c
    ${EVM_BASE}/components/evm/evm_module_watchdog.c
    ${EVM_BASE}/components/evm/evm_module_led.c
)

set(libevm_module_sources 
    ${evm_driver_sources}
    ${evm_modules_sources}
)

add_definitions("-DQMAKE_TARGET=\"ejs\"")
add_definitions("-DQMAKE_VERSION=\"1.0\"")
remove_definitions("-DWall")

include_directories(app PRIVATE ${EVM_BASE}/include)
include_directories(app PRIVATE ${EVM_BASE}/ejs)
include_directories(app PRIVATE src)

target_sources(app PRIVATE ${libevm_module_sources})
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE ${EVM_BASE}/ejs/evm_main.c)
target_sources(app PRIVATE ${EVM_BASE}/ejs/uol_output.c)
