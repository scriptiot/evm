
set(EVM_BASE ${ZEPHYR_BASE}/../../../)
include_directories(app PRIVATE ${EVM_BASE}/include)
include_directories(app PRIVATE ${EVM_BASE}/bsp/common)

zephyr_include_directories(.)
zephyr_library()
zephyr_library_sources( 
  ${EVM_BASE}/drivers/zephyr/uol/adc_driver.c 
  ${EVM_BASE}/drivers/zephyr/uol/can_driver.c 
  ${EVM_BASE}/drivers/zephyr/uol/dac_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/flash_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/gpio_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/i2c_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/rtc_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/spi_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/uart_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/watchdog_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/pwm_driver.c
  ${EVM_BASE}/drivers/zephyr/uol/sys_driver.c
)

if(CONFIG_EVM_MODULE_TIMER)
  zephyr_library_sources(evm_module_timer.c)
endif()


if(CONFIG_EVM_MODULE_ADC)
  zephyr_library_sources(evm_module_adc.c)
endif()
if(CONFIG_EVM_MODULE_CAN)
  zephyr_library_sources(evm_module_can.c)
endif()
if(CONFIG_EVM_MODULE_DAC)
  zephyr_library_sources(evm_module_dac.c)
endif()
if(CONFIG_EVM_MODULE_FLASH)
  zephyr_library_sources(evm_module_flash.c)
endif()
if(CONFIG_EVM_MODULE_I2C)
  zephyr_library_sources(evm_module_i2c.c)
endif()
if(CONFIG_EVM_MODULE_LCD)
  zephyr_library_sources(evm_module_lcd.c)
endif()
if(CONFIG_EVM_MODULE_LED)
  zephyr_library_sources(evm_module_led.c)
endif()
if(CONFIG_EVM_MODULE_MODBUS_RTU_MASTER)
  zephyr_library_sources(evm_module_modbus_rtu_master.c)
endif()
if(CONFIG_EVM_MODULE_NETWORK)
  zephyr_library_sources(evm_module_network.c)
endif()
if(CONFIG_EVM_MODULE_PIN)
  zephyr_library_sources(evm_module_pin.c)
endif()
if(CONFIG_EVM_MODULE_RTC)
  zephyr_library_sources(evm_module_rtc.c)
endif()
if(CONFIG_EVM_MODULE_SOCKET)
  zephyr_library_sources(evm_module_socket.c)
endif()
if(CONFIG_EVM_MODULE_SPI)
  zephyr_library_sources(evm_module_spi.c)
endif()
if(CONFIG_EVM_MODULE_SWITCH)
  zephyr_library_sources(evm_module_switch.c)
endif()

if(CONFIG_EVM_MODULE_UART)
  zephyr_library_sources(evm_module_uart.c)
endif()
if(CONFIG_EVM_MODULE_WATCHDOG)
  zephyr_library_sources(evm_module_watchdog.c)
endif()
if(CONFIG_EVM_MODULE_WIFI)
  zephyr_library_sources(evm_module_wifi.c)
endif()

zephyr_library_sources(evm_module.c)
