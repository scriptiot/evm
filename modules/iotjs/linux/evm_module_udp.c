#ifdef CONFIG_EVM_MODULE_UDP
#include "evm_module.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "netdb.h"

#define IPERF_PORT_LOCAL 5002
#define IPERF_PORT 10500
#define IPERF_BUFSZ (4 * 256)
#define IPERF_BUFSZ_UDP (1 * 256)

static evm_t *udp_obj_e;

typedef struct
{
    int sockfd;
    int obj_id;
} _evm_udp_t;

static void _udp_recv_thread(void *pvParameters)
{
    _evm_udp_t *sock = (_evm_udp_t *)pvParameters;
    if (sock == NULL || sock == 0)
        goto __exit;

    evm_val_t *obj = evm_module_registry_get(udp_obj_e, sock->obj_id);
    if (obj == NULL || obj == 0)
        goto __exit;

    int ret = 0, rec_len = sizeof(struct sockaddr_in);
    struct sockaddr_in cli_addr;
    while (1)
    {
        void * buf = evm_malloc(IPERF_BUFSZ_UDP);
        ret = recvfrom(sock->sockfd, buf, IPERF_BUFSZ_UDP, 0, (struct sockaddr *)&cli_addr, &rec_len);
        if (ret > 0)
        {
            evm_val_t *buffer = evm_buffer_create(udp_obj_e, (uint32_t)ret);
            if (buffer == NULL || buffer == 0) {
                evm_free(buf);
                goto __exit;
            }

            memcpy(evm_buffer_addr(buffer), buf, (size_t)ret);
            evm_val_t *ip = evm_heap_string_create(udp_obj_e, (const char *)inet_ntoa(cli_addr.sin_addr), INET_ADDRSTRLEN);
            evm_val_t *rinfo = evm_object_create(udp_obj_e, GC_DICT, 4, 0);
            evm_prop_append(udp_obj_e, rinfo, "address", *ip);
            evm_prop_append(udp_obj_e, rinfo, "family", evm_mk_foreign_string("IPv4"));
            evm_prop_append(udp_obj_e, rinfo, "port", evm_mk_number(ntohs(cli_addr.sin_port)));
            evm_prop_append(udp_obj_e, rinfo, "size", evm_mk_number(ret));

            evm_val_t args[2];
            args[0] = *buffer;
            args[1] = *rinfo;
            evm_module_event_emit(udp_obj_e, obj, "message", 2, args);
            evm_pop(udp_obj_e);
            break;
        }
        evm_free(buf);
        usleep(10);
    }

__exit:
    if (sock)
    {
        evm_module_registry_remove(udp_obj_e, sock->obj_id);
        close(sock->sockfd);
        evm_free(sock);
    }
}

//dgram.Socket.on
//Event: 'close'
//Event: 'error'
//Event: 'listening'
//Event: 'message'
static evm_val_t evm_module_udp_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setMulticastLoopback(flag)
static evm_val_t evm_module_udp_setMulticastLoopback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_boolean(v))
        return EVM_VAL_UNDEFINED;

    unsigned char loop = 0;
    if (evm_2_boolean(v))
        loop = 1;

    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (result < 0)
        evm_set_err(e, ec_type, "set multicast loopback failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setMulticastTTL(ttl)
static evm_val_t evm_module_udp_setMulticastTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    // 0~255, most system default value is 1
    unsigned char ttl = 255;
    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if (result < 0)
        evm_set_err(e, ec_type, "set multicast ttl failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setTTL(ttl)
static evm_val_t evm_module_udp_setTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    // 0-255, most system default value is 64
    int ttl = 0;
    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl));
    if (result < 0)
        evm_set_err(e, ec_type, "set ttl failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.addMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_addMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (argc > 1 && evm_is_string(v + 1))
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));

    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
    if (0 > result)
        evm_set_err(e, ec_type, "add membership failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.dropMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_dropMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (argc > 1 && evm_is_string(v + 1))
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));

    _evm_udp_t *sock = (_evm_udp_t *)evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int len = 0;
    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, &len);
    if (0 > result)
        evm_set_err(e, ec_type, "drop membership failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.address()
//address, port and family
static evm_val_t evm_module_udp_address(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    evm_val_t *result = evm_object_create(e, GC_DICT, 3, 0);
    struct sockaddr addr;
    struct sockaddr_in *addr_in;
    int len = sizeof(struct sockaddr_in);

    if (0 == getsockname(sock->sockfd, &addr, &len))
    {
        addr_in = (struct sockaddr_in *)&addr;

        if (addr.sa_family != AF_INET)
            return EVM_VAL_UNDEFINED;

        uint8_t *ip_str = (uint8_t *)evm_malloc(INET_ADDRSTRLEN);
        evm_val_t *ip = evm_heap_string_create(e, "", INET_ADDRSTRLEN);
        memset(ip_str, 0, INET_ADDRSTRLEN);
        inet_ntop(addr_in->sin_family, &addr_in->sin_addr, (char *)ip_str, INET_ADDRSTRLEN);
        evm_heap_string_set(e, ip, ip_str, 0, INET_ADDRSTRLEN);
        evm_prop_append(e, result, "family", evm_mk_foreign_string("IPv4"));
        evm_prop_append(e, result, "address", *ip);
        evm_prop_append(e, result, "port", evm_mk_number(ntohs(addr_in->sin_port)));
        evm_free(ip_str);
    }

    return *result;
}

//dgram.Socket.bind([port][, address][, callback])
//dgram.Socket.bind(options[, callback]) not support
static evm_val_t evm_module_udp_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    struct sockaddr_in laddr;
    memset(&laddr, 0, sizeof(laddr));
    laddr.sin_family = PF_INET;
    laddr.sin_port = htons(IPERF_PORT_LOCAL);
    laddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (argc > 0 && evm_is_number(v))
        laddr.sin_port = htons((uint16_t)evm_2_integer(v));

    if (argc > 1 && evm_is_string(v + 1))
        laddr.sin_addr.s_addr = inet_addr(evm_2_string(v + 1));

    int ret = bind(sock->sockfd, (struct sockaddr *)&laddr, sizeof(laddr));
    if (ret < 0)
    {
        close(sock->sockfd);
        evm_free(sock);
        evm_set_err(e, ec_type, "bind error\n");
        return EVM_VAL_UNDEFINED;
    }

    //emit event of listening
    evm_module_event_emit(e, p, "listening", 0, NULL);

    if (argc > 2 && evm_is_script(v + 2))
        evm_run_callback(e, &e->scope, v + 2, NULL, 0);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *)_udp_recv_thread, sock);

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.close([callback])
static evm_val_t evm_module_udp_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int result = close(sock->sockfd);
    if (result < 0)
        evm_set_err(e, ec_type, "close socket failed\n");

    if (argc > 0 && evm_is_script(v))
        evm_run_callback(e, &e->scope, v, NULL, 0);

    //emit close event
    evm_module_event_emit(e, p, "close", 0, NULL);
    evm_free(sock);

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.send(msg, [offset, length,] port [, address] [, callback])
static evm_val_t evm_module_udp_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 5 || !(evm_is_string(v) || evm_is_buffer(v)) || !evm_is_integer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_string(v + 4))
        return EVM_VAL_UNDEFINED;

    int port = evm_2_integer(v + 3);
    const char *ip = evm_2_string(v + 4);

    /*complete the struct: sockaddr_in*/
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t)port);
    server.sin_addr.s_addr = inet_addr(ip);

    /* send the string to server*/
    const char *msg = NULL;
    int len = 0;
    if (evm_is_string(v))
    {
        msg = (const char *)evm_2_string(v);
        len = (int)evm_string_len(v);
    }
    else
    {
        msg = (const char *)evm_buffer_addr(v);
        len = (int)evm_buffer_len(v);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        return EVM_VAL_UNDEFINED;

    if (sendto(sockfd, msg, (size_t)len, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        evm_set_err(e, ec_type, "sendto error\r\n");
        return EVM_VAL_UNDEFINED;
    }

    if (argc > 5 && evm_is_script(v + 5))
        evm_run_callback(e, &e->scope, v + 5, NULL, 0);

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setBroadcast(flag)
static evm_val_t evm_module_udp_setBroadcast(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_boolean(v))
        return EVM_VAL_UNDEFINED;

    int so_broadcast = 0;
    if (evm_2_boolean(v))
        so_broadcast = 1;

    _evm_udp_t *sock = evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(sock->sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
    if (result < 0)
        evm_set_err(e, ec_type, "set broadcast failed\n");

    return EVM_VAL_UNDEFINED;
}

//dgram.createSocket(options[, callback])
//dgram.createSocket(type[, callback])
static evm_val_t evm_module_udp_createSocket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (!(argc > 0 && (evm_is_string(v) || evm_is_object(v))))
        return EVM_VAL_UNDEFINED;

    int domain = -1;
    int reuseAddr = 1;
    if (evm_is_object(v))
    {
        evm_val_t *t = evm_prop_get(e, v, "type", 0);
        if (t == NULL)
            return EVM_VAL_UNDEFINED;

        if (strncmp(evm_2_string(t), "udp4", 4) == 0)
            domain = AF_INET;

        if (strncmp(evm_2_string(t), "udp6", 4) == 0)
            domain = AF_INET6;

        if (domain == -1)
            return EVM_VAL_UNDEFINED;

        evm_val_t *reuse_addr = evm_prop_get(e, v, "reuseAddr", 0);
        if (evm_is_boolean(reuse_addr))
            reuseAddr = evm_2_boolean(reuse_addr);
    } else {
        if (strncmp(evm_2_string(v), "udp4", 4) == 0)
            domain = AF_INET;
        else if (strncmp(evm_2_string(v), "udp6", 4) == 0)
            domain = AF_INET6;

        if (domain == -1)
            return EVM_VAL_UNDEFINED;
    }

    int sockfd = socket(domain, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        return EVM_VAL_UNDEFINED;

    int one = 1;
    if (reuseAddr && setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
        return EVM_VAL_UNDEFINED;

    evm_val_t *obj = evm_object_create(e, GC_DICT, 11, 0);
    if (obj == NULL || obj == 0)
        return EVM_VAL_UNDEFINED;

    evm_prop_append(e, obj, "addMembership", evm_mk_native((intptr_t)evm_module_udp_addMembership));
    evm_prop_append(e, obj, "address", evm_mk_native((intptr_t)evm_module_udp_address));
    evm_prop_append(e, obj, "bind", evm_mk_native((intptr_t)evm_module_udp_bind));
    evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_udp_close));
    evm_prop_append(e, obj, "dropMembership", evm_mk_native((intptr_t)evm_module_udp_dropMembership));
    evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_udp_on));
    evm_prop_append(e, obj, "send", evm_mk_native((intptr_t)evm_module_udp_send));
    evm_prop_append(e, obj, "setBroadcast", evm_mk_native((intptr_t)evm_module_udp_setBroadcast));
    evm_prop_append(e, obj, "setMulticastLoopback", evm_mk_native((intptr_t)evm_module_udp_setMulticastLoopback));
    evm_prop_append(e, obj, "setMulticastTTL", evm_mk_native((intptr_t)evm_module_udp_setMulticastTTL));
    evm_prop_append(e, obj, "setTTL", evm_mk_native((intptr_t)evm_module_udp_setTTL));

    _evm_udp_t *sock = (_evm_udp_t *)evm_malloc(sizeof(_evm_udp_t));
    if (sock == NULL || sock == 0)
    {
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }

    sock->sockfd = sockfd;
    sock->obj_id = evm_module_registry_add(e, obj);
    evm_object_set_ext_data(obj, (intptr_t)sock);

    return *obj;
}

evm_err_t evm_module_udp(evm_t *e)
{
    udp_obj_e = e;
    evm_builtin_t builtin[] = {
        {"createSocket", evm_mk_native((intptr_t)evm_module_udp_createSocket)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "dgram", builtin);
    return e->err;
}

#endif
