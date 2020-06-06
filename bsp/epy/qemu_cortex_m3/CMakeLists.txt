# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(eqml)

set(EVM_BASE ${ZEPHYR_BASE}/../../../)


set(LIBEPY ${EVM_BASE}/lib/arm-none-eabi-gcc/cortex-m3/libepy.a)
message(${LIBEPY})
target_link_libraries(app PRIVATE ${LIBEPY})

set(evm_python_builtins_sources 
    ${EVM_BASE}/modules/python_builtins/python_builtins.c
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
    ${evm_python_builtins_sources}
)

include_directories(app PRIVATE ${EVM_BASE}/include)
include_directories(app PRIVATE ${EVM_BASE}/bsp/common)
include_directories(app PRIVATE ${EVM_BASE}/modules/python_builtins)
include_directories(app PRIVATE src)

target_sources(app PRIVATE ${libevm_module_sources})
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE ${EVM_BASE}/bsp/common/evm_main.c)
target_sources(app PRIVATE ${EVM_BASE}/bsp/common/uol_output.c)

# language config
add_definitions("-DCONFIG_EVM_LANG_PY")
