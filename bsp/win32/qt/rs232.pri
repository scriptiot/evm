INCLUDEPATH += $$PWD/../rock/librs232/include
INCLUDEPATH += $$PWD/../rock/librs232/include/librs232

HEADERS += \
    $$PWD/../rock/librs232/include/librs232/rs232.h \
    $$PWD/../rock/librs232/include/librs232/rs232_windows.h

SOURCES += \
    $$PWD/../rock/librs232/bindings/lua/luars232.c \
    $$PWD/../rock/librs232/src/rs232.c \
    $$PWD/../rock/librs232/src/rs232_windows.c
