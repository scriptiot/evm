SOURCES += \
    $$PWD/../port/luat_base_win32.c \
    $$PWD/../port/luat_fs_win32.c \
    $$PWD/../port/luat_log_win32.c \
    $$PWD/../port/luat_luadb_inline.c \
    $$PWD/../port/luat_malloc_win32.c \
    $$PWD/../port/luat_msgbus_freertos.c \
    $$PWD/../port/luat_lib_gpio.c \
    $$PWD/../port/luat_gpio_rtt.c \
    $$PWD/../port/luat_http_win32.c \
    $$PWD/../port/luat_timer_freertos.c

win32 {
#SOURCES += \
#    $$PWD/../port/luat_crypto_win32.c
}

unix {
SOURCES += \
    $$PWD/../port/luat_lib_socket.c \
    $$PWD/../port/luat_socket_rtt.c \
    $$PWD/../port/luat_netclient_rtt.c
}
