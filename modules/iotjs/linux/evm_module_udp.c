#ifdef CONFIG_EVM_MODULE_UDP
#include "evm_module.h"
#include <sys/socket.h>

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
static evm_val_t evm_module_udp_dgram_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
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
        evm_prop_append(e, obj, "addMembership", evm_mk_native((intptr_t)evm_module_udp_class_addMembership));
        evm_prop_append(e, obj, "address", evm_mk_native((intptr_t)evm_module_udp_class_address));
        evm_prop_append(e, obj, "bind", evm_mk_native((intptr_t)evm_module_udp_class_bind));
        evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_udp_class_close));
        evm_prop_append(e, obj, "dropMembership", evm_mk_native((intptr_t)evm_module_udp_class_dropMembership));
        evm_prop_append(e, obj, "send", evm_mk_native((intptr_t)evm_module_udp_class_send));
        evm_prop_append(e, obj, "sendto", evm_mk_native((intptr_t)evm_module_udp_class_sendto));
        evm_prop_append(e, obj, "setBroadcast", evm_mk_native((intptr_t)evm_module_udp_class_closeSync));
        evm_prop_append(e, obj, "setMulticastLoopback", evm_mk_native((intptr_t)evm_module_udp_class_setMulticastLoopback));
        evm_prop_append(e, obj, "setMulticastTTL", evm_mk_native((intptr_t)evm_module_udp_class_setMulticastTTL));
        evm_prop_append(e, obj, "setTTL", evm_mk_native((intptr_t)evm_module_udp_class_setTTL));
    }
    return obj;
}

evm_err_t evm_module_udp(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"createSocket", evm_mk_native((intptr_t)evm_module_udp_dgram_createSocket)},
        {"on", evm_mk_native((intptr_t)evm_module_udp_dgram_on)},
        {NULL, NULL}};
    evm_module_create(e, "dgram", builtin);
    return e->err;
}
#endif
