TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += LINUX
DEFINES += EVM_LANG_ENABLE_REPL
DEFINES += EVM_LANG_ENABLE_JAVASCRIPT



#DEFINES += CONFIG_EVM_MODULE_FS
DEFINES += CONFIG_EVM_MODULE_NET
DEFINES += CONFIG_EVM_MODULE_PROCESS
DEFINES += CONFIG_EVM_MODULE_EVENTS
DEFINES += CONFIG_EVM_MODULE_DNS
DEFINES += CONFIG_EVM_MODULE_TIMERS
DEFINES += CONFIG_EVM_MODULE_BUFFER
DEFINES += CONFIG_EVM_MODULE_ASSERT
DEFINES += CONFIG_EVM_MODULE_HTTP
DEFINES += CONFIG_EVM_MODULE_MPY

if(contains(DEFINES, CONFIG_EVM_MODULE_MPY)) {
    QMAKE_CFLAGS += -m32
    QMAKE_LFLAGS += -m32
SOURCES += \
    ../../../components/micropython/py/argcheck.c \
    ../../../components/micropython/py/asmarm.c \
    ../../../components/micropython/py/asmbase.c \
    ../../../components/micropython/py/asmthumb.c \
    ../../../components/micropython/py/asmx64.c \
    ../../../components/micropython/py/asmx86.c \
    ../../../components/micropython/py/asmxtensa.c \
    ../../../components/micropython/py/bc.c \
    ../../../components/micropython/py/binary.c \
    ../../../components/micropython/py/builtinevex.c \
    ../../../components/micropython/py/builtinhelp.c \
    ../../../components/micropython/py/builtinimport.c \
    ../../../components/micropython/py/compile.c \
    ../../../components/micropython/py/emitbc.c \
    ../../../components/micropython/py/emitcommon.c \
    ../../../components/micropython/py/emitglue.c \
    ../../../components/micropython/py/emitinlinethumb.c \
    ../../../components/micropython/py/emitinlinextensa.c \
    ../../../components/micropython/py/emitnarm.c \
    ../../../components/micropython/py/emitnative.c \
    ../../../components/micropython/py/emitnthumb.c \
    ../../../components/micropython/py/emitnx64.c \
    ../../../components/micropython/py/emitnx86.c \
    ../../../components/micropython/py/emitnxtensa.c \
    ../../../components/micropython/py/emitnxtensawin.c \
    ../../../components/micropython/py/formatfloat.c \
    ../../../components/micropython/py/frozenmod.c \
    ../../../components/micropython/py/gc.c \
    ../../../components/micropython/py/lexer.c \
    ../../../components/micropython/py/malloc.c \
    ../../../components/micropython/py/map.c \
    ../../../components/micropython/py/modarray.c \
    ../../../components/micropython/py/modbuiltins.c \
    ../../../components/micropython/py/modcmath.c \
    ../../../components/micropython/py/modcollections.c \
    ../../../components/micropython/py/modgc.c \
    ../../../components/micropython/py/modio.c \
    ../../../components/micropython/py/modmath.c \
    ../../../components/micropython/py/modmicropython.c \
    ../../../components/micropython/py/modstruct.c \
    ../../../components/micropython/py/modsys.c \
    ../../../components/micropython/py/modthread.c \
    ../../../components/micropython/py/moduerrno.c \
    ../../../components/micropython/py/mpprint.c \
    ../../../components/micropython/py/mpstate.c \
    ../../../components/micropython/py/mpz.c \
    ../../../components/micropython/py/nativeglue.c \
    ../../../components/micropython/py/nlr.c \
    ../../../components/micropython/py/nlraarch64.c \
    ../../../components/micropython/py/nlrpowerpc.c \
    ../../../components/micropython/py/nlrsetjmp.c \
    ../../../components/micropython/py/nlrthumb.c \
    ../../../components/micropython/py/nlrx64.c \
    ../../../components/micropython/py/nlrx86.c \
    ../../../components/micropython/py/nlrxtensa.c \
    ../../../components/micropython/py/obj.c \
    ../../../components/micropython/py/objarray.c \
    ../../../components/micropython/py/objattrtuple.c \
    ../../../components/micropython/py/objbool.c \
    ../../../components/micropython/py/objboundmeth.c \
    ../../../components/micropython/py/objcell.c \
    ../../../components/micropython/py/objclosure.c \
    ../../../components/micropython/py/objcomplex.c \
    ../../../components/micropython/py/objdeque.c \
    ../../../components/micropython/py/objdict.c \
    ../../../components/micropython/py/objenumerate.c \
    ../../../components/micropython/py/objexcept.c \
    ../../../components/micropython/py/objfilter.c \
    ../../../components/micropython/py/objfloat.c \
    ../../../components/micropython/py/objfun.c \
    ../../../components/micropython/py/objgenerator.c \
    ../../../components/micropython/py/objgetitemiter.c \
    ../../../components/micropython/py/objint_longlong.c \
    ../../../components/micropython/py/objint_mpz.c \
    ../../../components/micropython/py/objint.c \
    ../../../components/micropython/py/objlist.c \
    ../../../components/micropython/py/objmap.c \
    ../../../components/micropython/py/objmodule.c \
    ../../../components/micropython/py/objnamedtuple.c \
    ../../../components/micropython/py/objnone.c \
    ../../../components/micropython/py/objobject.c \
    ../../../components/micropython/py/objpolyiter.c \
    ../../../components/micropython/py/objproperty.c \
    ../../../components/micropython/py/objrange.c \
    ../../../components/micropython/py/objreversed.c \
    ../../../components/micropython/py/objset.c \
    ../../../components/micropython/py/objsingleton.c \
    ../../../components/micropython/py/objslice.c \
    ../../../components/micropython/py/objstr.c \
    ../../../components/micropython/py/objstringio.c \
    ../../../components/micropython/py/objstrunicode.c \
    ../../../components/micropython/py/objtuple.c \
    ../../../components/micropython/py/objtype.c \
    ../../../components/micropython/py/objzip.c \
    ../../../components/micropython/py/opmethods.c \
    ../../../components/micropython/py/pairheap.c \
    ../../../components/micropython/py/parse.c \
    ../../../components/micropython/py/parsenum.c \
    ../../../components/micropython/py/parsenumbase.c \
    ../../../components/micropython/py/persistentcode.c \
    ../../../components/micropython/py/profile.c \
    ../../../components/micropython/py/pystack.c \
    ../../../components/micropython/py/qstr.c \
    ../../../components/micropython/py/reader.c \
    ../../../components/micropython/py/repl.c \
    ../../../components/micropython/py/ringbuf.c \
    ../../../components/micropython/py/runtime_utils.c \
    ../../../components/micropython/py/runtime.c \
    ../../../components/micropython/py/scheduler.c \
    ../../../components/micropython/py/scope.c \
    ../../../components/micropython/py/sequence.c \
    ../../../components/micropython/py/showbc.c \
    ../../../components/micropython/py/smallint.c \
    ../../../components/micropython/py/stackctrl.c \
    ../../../components/micropython/py/stream.c \
    ../../../components/micropython/py/unicode.c \
    ../../../components/micropython/py/vm.c \
    ../../../components/micropython/py/vstr.c \
    ../../../components/micropython/py/warning.c \
    ../../../modules/mpy/evm_module_mpy.c \
    ../../../modules/mpy/_frozen_mpy.c \
    ../../../modules/mpy/uart_core.c \
    ../../../components/micropython/lib/utils/pyexec.c \
    ../../../components/micropython/lib/utils/stdout_helpers.c \
    ../../../components/micropython/lib/utils/printf.c \
    ../../../components/micropython/lib/mp-readline/readline.c

HEADERS += \
    ../../../components/micropython/py/asmarm.h \
    ../../../components/micropython/py/asmbase.h \
    ../../../components/micropython/py/asmthumb.h \
    ../../../components/micropython/py/asmx64.h \
    ../../../components/micropython/py/asmx86.h \
    ../../../components/micropython/py/asmxtensa.h \
    ../../../components/micropython/py/bc.h \
    ../../../components/micropython/py/bc0.h \
    ../../../components/micropython/py/binary.h \
    ../../../components/micropython/py/builtin.h \
    ../../../components/micropython/py/compile.h \
    ../../../components/micropython/py/dynruntime.h \
    ../../../components/micropython/py/emit.h \
    ../../../components/micropython/py/emitglue.h \
    ../../../components/micropython/py/formatfloat.h \
    ../../../components/micropython/py/frozenmod.h \
    ../../../components/micropython/py/gc.h \
    ../../../components/micropython/py/grammar.h \
    ../../../components/micropython/py/lexer.h \
    ../../../components/micropython/py/misc.h \
    ../../../components/micropython/py/mpconfig.h \
    ../../../components/micropython/py/mperrno.h \
    ../../../components/micropython/py/mphal.h \
    ../../../components/micropython/py/mpprint.h \
    ../../../components/micropython/py/mpstate.h \
    ../../../components/micropython/py/mpthread.h \
    ../../../components/micropython/py/mpz.h \
    ../../../components/micropython/py/nativeglue.h \
    ../../../components/micropython/py/nlr.h \
    ../../../components/micropython/py/obj.h \
    ../../../components/micropython/py/objarray.h \
    ../../../components/micropython/py/objexcept.h \
    ../../../components/micropython/py/objfun.h \
    ../../../components/micropython/py/objgenerator.h \
    ../../../components/micropython/py/objint.h \
    ../../../components/micropython/py/objlist.h \
    ../../../components/micropython/py/objmodule.h \
    ../../../components/micropython/py/objnamedtuple.h \
    ../../../components/micropython/py/objstr.h \
    ../../../components/micropython/py/objstringio.h \
    ../../../components/micropython/py/objtuple.h \
    ../../../components/micropython/py/objtype.h \
    ../../../components/micropython/py/pairheap.h \
    ../../../components/micropython/py/parse.h \
    ../../../components/micropython/py/parsenum.h \
    ../../../components/micropython/py/parsenumbase.h \
    ../../../components/micropython/py/persistentcode.h \
    ../../../components/micropython/py/profile.h \
    ../../../components/micropython/py/pystack.h \
    ../../../components/micropython/py/qstr.h \
    ../../../components/micropython/py/qstrdefs.h \
    ../../../components/micropython/py/reader.h \
    ../../../components/micropython/py/repl.h \
    ../../../components/micropython/py/ringbuf.h \
    ../../../components/micropython/py/runtime.h \
    ../../../components/micropython/py/runtime0.h \
    ../../../components/micropython/py/scope.h \
    ../../../components/micropython/py/smallint.h \
    ../../../components/micropython/py/stackctrl.h \
    ../../../components/micropython/py/stream.h \
    ../../../components/micropython/py/unicode.h \
    ../../../components/micropython/py/vmentrytable.h

    LIBS += -L$$PWD/../../../lib/x86/linux32 -lecma -lejs
} else {
    LIBS += -L$$PWD/../../../lib/x86/linux -lecma -lejs
}

LIBS += -lpthread
LIBS += -lrt
INCLUDEPATH += $$PWD/../../../include
INCLUDEPATH += $$PWD/../../../components/webclient/inc
INCLUDEPATH += $$PWD/../../../components/micropython
INCLUDEPATH += $$PWD/../../../modules/mpy

SOURCES += \
        main.c \
    ../../../modules/iotjs/linux/evm_main.c \
    ../../../modules/iotjs/linux/evm_module_net.c \
    ../../../modules/iotjs/common/evm_module_process.c \
    ../../../modules/iotjs/common/evm_module.c \
    ../../../modules/iotjs/linux/evm_module_fs.c \
    ../../../modules/iotjs/common/evm_module_events.c \
    ../../../modules/iotjs/linux/evm_module_dns.c \
    ../../../modules/iotjs/linux/evm_module_timers.c \
    ../../../modules/iotjs/common/evm_module_buffer.c \
    ../../../modules/iotjs/common/evm_module_assert.c \
    ../../../modules/iotjs/linux/evm_module_http.c \
    ../../../components/webclient/src/webclient.c


