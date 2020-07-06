
set(EVM_BASE ${ZEPHYR_BASE}/../../../)

zephyr_include_directories(.)
zephyr_include_directories(${EVM_BASE}/include)

if(CONFIG_STM32_DISPLAY_ILI9341)
  zephyr_include_directories(display/stm32_hal)
  zephyr_library_sources(display/stm32_hal/display_ili9341.c)
endif()

if(CONFIG_ETH_W5500)
  zephyr_include_directories(ethernet)
  zephyr_include_directories(ethernet/w5500/ioLibrary/Ethernet)
  zephyr_library_sources(
      ethernet/w5500/ioLibrary/Ethernet/wizchip_conf.c
      ethernet/w5500/ioLibrary/Ethernet/W5500/w5500.c
      ethernet/w5500/driver_w5500.c
  )
endif()
