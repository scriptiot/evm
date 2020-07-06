if(CONFIG_EVM_ECMA)
  set(EVM_BASE ${ZEPHYR_BASE}/../../../)
  include_directories(app PRIVATE ${EVM_BASE}/include)

  zephyr_include_directories(.)
  zephyr_include_directories(${EVM_BASE}/include)

if(CONFIG_EVM_ECMA_ARRAY)
  zephyr_library_sources(ecma_array.c)
endif()

if(CONFIG_EVM_ECMA_MATH)
  zephyr_library_sources(ecma_math.c)
endif()

if(CONFIG_EVM_ECMA_NUMBER)
  zephyr_library_sources(ecma_number.c)
endif()

if(CONFIG_EVM_ECMA_OBJECT)
  zephyr_library_sources(ecma_object.c)
endif()

if(CONFIG_EVM_ECMA_REGEXP)
  zephyr_library_sources(ecma_regexp.c)
endif()

if(CONFIG_EVM_ECMA_STRING)
  zephyr_library_sources(ecma_string.c)
endif()

  zephyr_library_sources(ecma.c)

endif()
