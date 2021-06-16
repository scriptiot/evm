HEADERS += \
    $$PWD/../../../components/freertos/include/FreeRTOS.h \
    $$PWD/../../../components/freertos/include/FreeRTOSConfig.h \
    $$PWD/../../../components/freertos/include/StackMacros.h \
    $$PWD/../../../components/freertos/include/atomic.h \
    $$PWD/../../../components/freertos/include/croutine.h \
    $$PWD/../../../components/freertos/include/deprecated_definitions.h \
    $$PWD/../../../components/freertos/include/event_groups.h \
    $$PWD/../../../components/freertos/include/list.h \
    $$PWD/../../../components/freertos/include/message_buffer.h \
    $$PWD/../../../components/freertos/include/mpu_prototypes.h \
    $$PWD/../../../components/freertos/include/mpu_wrappers.h \
    $$PWD/../../../components/freertos/include/portable.h \
    $$PWD/../../../components/freertos/include/projdefs.h \
    $$PWD/../../../components/freertos/include/queue.h \
    $$PWD/../../../components/freertos/include/semphr.h \
    $$PWD/../../../components/freertos/include/stack_macros.h \
    $$PWD/../../../components/freertos/include/stdint.readme \
    $$PWD/../../../components/freertos/include/stream_buffer.h \
    $$PWD/../../../components/freertos/include/task.h \
    $$PWD/../../../components/freertos/include/timers.h

SOURCES += \
    $$PWD/../../../components/freertos/croutine.c \
    $$PWD/../../../components/freertos/event_groups.c \
    $$PWD/../../../components/freertos/heap_4.c \
    $$PWD/../../../components/freertos/list.c \
    $$PWD/../../../components/freertos/queue.c \
    $$PWD/../../../components/freertos/stream_buffer.c \
    $$PWD/../../../components/freertos/tasks.c \
    $$PWD/../../../components/freertos/timers.c


win32 {
HEADERS += \
    $$PWD/../../../components/freertos/portable/MSVC-MingW/portmacro.h

SOURCES += \
    $$PWD/../../../components/freertos/portable/MSVC-MingW/port.c
}


unix {
HEADERS += \
    $$PWD/../../../components/freertos/portable/ThirdParty/GCC/Posix/portmacro.h \
    $$PWD/../../../components/freertos/portable/ThirdParty/GCC/Posix/utils/wait_for_event.h
SOURCES += \
    $$PWD/../../../components/freertos/portable/ThirdParty/GCC/Posix/port.c \
    $$PWD/../../../components/freertos/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \
}
