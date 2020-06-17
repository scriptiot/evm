if(CONFIG_EVM_HEATSHRINK)
  set(EVM_BASE ${ZEPHYR_BASE}/../../../)
  include_directories(app PRIVATE ${EVM_BASE}/include)
  include_directories(app PRIVATE ${EVM_BASE}/components/heatshrink)

  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    compress_heatshrink.c
    wrap_heatshrink.c
    ${EVM_BASE}/components/heatshrink/heatshrink_decoder.c
    ${EVM_BASE}/components/heatshrink/heatshrink_encoder.c
    )
endif()
