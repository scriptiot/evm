#ifdef CONFIG_EVM_MODULE_UDP
#include "evm_module.h"

evm_val_t *evm_module_udp_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//dgram.createSocket(options[, callback])
//dgram.createSocket(type[, callback])
static evm_val_t evm_module_udp_dgram_createSocket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//dgram.on(event, callback)
//Event:
//  close
//  error
//  listening
//  message
static evm_val_t evm_module_udp_dgram_onEvent(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.addMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_class_addMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.address()
static evm_val_t evm_module_udp_class_address(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.bind([port][, address][, bindListener])
//socket.bind(options[, bindListener])
static evm_val_t evm_module_udp_class_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.close([closeListener])
static evm_val_t evm_module_udp_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.dropMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_class_dropMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.setBroadcast(flag)
static evm_val_t evm_module_udp_class_setBroadcast(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.send(msg, [offset, length,] port [, address] [, sendListener])
static evm_val_t evm_module_udp_class_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.sendto(msg, offset, length, port [, address] [, sendListener])
static evm_val_t evm_module_udp_class_sendto(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.setMulticastLoopback(flag)
static evm_val_t evm_module_udp_class_setMulticastLoopback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.setMulticastTTL(ttl)
static evm_val_t evm_module_udp_class_setMulticastTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//socket.setTTL(ttl)
static evm_val_t evm_module_udp_class_setTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_udp_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 6, 0);
    if (obj)
    {
        evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_udp_class_read));
        evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_udp_class_readSync));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_udp_class_write));
        evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_udp_class_writeSync));
        evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_udp_class_close));
        evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_udp_class_closeSync));
    }
    return obj;
}

evm_err_t evm_module_udp(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"open", evm_mk_native((intptr_t)evm_module_udp_open)},
        {"openSync", evm_mk_native((intptr_t)evm_module_udp_openSync)},
        {NULL, NULL}};
    evm_module_create(e, "dgram", builtin);
    return e->err;
}
#endif
