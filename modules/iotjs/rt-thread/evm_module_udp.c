#ifdef CONFIG_EVM_MODULE_UDP
#include "evm_module.h"
#include <rtthread.h>
#include <sys/socket.h>
#include "netdb.h"

evm_val_t *evm_module_udp_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//dgram.createSocket(options[, callback])
//dgram.createSocket(type[, callback])
static evm_val_t evm_module_udp_dgram_createSocket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{

    s = socket(AF_INET, SOCK_DGRAM, 0); /*建立套接字*/
    if (s == -1)
    {
        perror("socket()");
        return -1;
    }

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
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v)); // MCAST_ADDR
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (argc > 1 && evm_is_string(v + 1))
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));

    int s = evm_object_get_ext_data(p);
    if (!s)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "setsockopt():IP_ADD_MEMBERSHIP");
        return EVM_VAL_NULL;
    }
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
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    struct sockaddr_in local_addr;
    bzero(&local_addr, sizeof(struct sockaddr_in));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    local_addr.sin_port = htons(evm_2_integer(v));

    if (argc > 1 && evm_is_string(v + 1))
    {
        local_addr.sin_addr.s_addr = htonl(evm_2_string(v + 1));
    }

    int s = evm_object_get_ext_data(p);
    if (!s)
        return EVM_VAL_UNDEFINED;

    int result = bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "bind()");
        return EVM_VAL_NULL;
    }
    return EVM_VAL_UNDEFINED;
}

//socket.close([closeListener])
static evm_val_t evm_module_udp_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int s = evm_object_get_ext_data(p);
    if (!s)
        return EVM_VAL_UNDEFINED;

    close(s);
    return EVM_VAL_UNDEFINED;
}

//socket.dropMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_class_dropMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v)); // MCAST_ADDR
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (argc > 1 && evm_is_string(v + 1))
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));

    int s = evm_object_get_ext_data(p);
    if (!s)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "setsockopt():IP_DROP_MEMBERSHIP");
        return EVM_VAL_NULL;
    }
    return EVM_VAL_UNDEFINED;
}

//socket.setBroadcast(flag)
static evm_val_t evm_module_udp_class_setBroadcast(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int flag = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));
    return EVM_VAL_UNDEFINED;
}

//socket.send(msg, [offset, length,] port [, address] [, sendListener])
static evm_val_t evm_module_udp_class_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    send(int sockfd, const void *buf, size_t len, int flags);
    return EVM_VAL_UNDEFINED;
}

//socket.sendto(msg, offset, length, port [, address] [, sendListener])
static evm_val_t evm_module_udp_class_sendto(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int s = evm_object_get_ext_data(p);
    if (!s)
        return EVM_VAL_UNDEFINED;

    int result = sendto(s, MCAST_DATA, sizeof(MCAST_DATA), 0, (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "sendto()");
        return EVM_VAL_NULL;
    }
    return EVM_VAL_UNDEFINED;
}

//socket.setMulticastLoopback(flag)
static evm_val_t evm_module_udp_class_setMulticastLoopback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    /*设置回环许可*/
    int loop = 1;
    err = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (err < 0)
    {
        perror("setsockopt():IP_MULTICAST_LOOP");
        return -3;
    }

    return EVM_VAL_UNDEFINED;
}

//socket.setMulticastTTL(ttl)
static evm_val_t evm_module_udp_class_setMulticastTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
    return EVM_VAL_UNDEFINED;
}

//socket.setTTL(ttl)
static evm_val_t evm_module_udp_class_setTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    setsockopt(s, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl));
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
