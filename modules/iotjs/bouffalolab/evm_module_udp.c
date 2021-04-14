#ifdef CONFIG_EVM_MODULE_UDP
#include "evm_module.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
#define IPERF_PORT_LOCAL 5002
#define IPERF_PORT 10500
#define IPERF_BUFSZ (4 * 256)
#define IPERF_BUFSZ_UDP (1 * 256)

typedef struct
{
    int sockfd;
    int obj_id;
} _evm_udp_t;

static void _udp_recv_thread(void *pvParameters)
{
    evm_val_t *buffer = evm_buffer_create(evm_runtime, IPERF_BUFSZ_UDP);
    if (buffer == NULL || buffer == 0)
    {
        goto __exit;
    }

    _evm_udp_t *sock = (_evm_udp_t *)pvParameters;
    if (sock == NULL || sock == 0)
    {
        goto __exit;
    }

    evm_val_t *obj = evm_module_registry_get(evm_runtime, sock->obj_id);
    if (obj == NULL || obj == 0)
    {
        goto __exit;
    }

    int ret = 0;
    while (1) // 启动一个线程处理
    {
        memset(evm_buffer_addr(buffer), 0, sizeof(IPERF_BUFSZ_UDP));
        ret = recv(sock->sockfd, evm_buffer_addr(buffer), IPERF_BUFSZ_UDP, 0);
        if (ret > 0)
        {
            //emit message callback event
            evm_module_event_emit(evm_runtime, obj, "message", 1, buffer);
            break;
        }
        vTaskDelay(10);
    }

__exit:
    if (sock)
    {
        lwip_close(sock->sockfd);
        evm_free(sock);
        vTaskDelete(NULL);
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
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setMulticastLoopback(flag)
static evm_val_t evm_module_udp_setMulticastLoopback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_boolean(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    unsigned char loop = 0;
    if (evm_2_boolean(v))
    {
        loop = 1;
    }

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(*sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "set multicast loopback failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setMulticastTTL(ttl)
static evm_val_t evm_module_udp_setMulticastTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    // 0~255, most system default value is 1
    unsigned char ttl = 255;
    int result = setsockopt(*sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "set multicast ttl failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setTTL(ttl)
static evm_val_t evm_module_udp_setTTL(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    // 0-255, most system default value is 64
    int ttl = 0;
    int result = setsockopt(*sockfd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "set ttl failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.addMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_addMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    struct ip_mreq mreq;
    //IP multicast address of group
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v));
    //Local IP address of interface
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (argc > 1 && evm_is_string(v + 1))
    {
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));
    }

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(*sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
    if (0 > result)
    {
        evm_set_err(e, ec_type, "add membership failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.dropMembership(multicastAddress[, multicastInterface])
static evm_val_t evm_module_udp_dropMembership(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(evm_2_string(v));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (argc > 1 && evm_is_string(v + 1))
    {
        mreq.imr_interface.s_addr = htonl(evm_2_string(v + 1));
    }

    _evm_udp_t *sock = (_evm_udp_t *)evm_object_get_ext_data(p);
    if (sock == NULL || sock == 0)
        return EVM_VAL_UNDEFINED;

    int len = 0;
    int result = setsockopt(sock->sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, &len);
    if (0 > result)
    {
        evm_set_err(e, ec_type, "drop membership failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.address()
//address, port and family
static evm_val_t evm_module_udp_address(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    evm_val_t *result = evm_object_create(e, GC_DICT, 3, 0);
    struct sockaddr addr;
    struct sockaddr_in *addr_in;
    int len = sizeof(*addr_in);

    if (0 == getsockname(*sockfd, &addr, &len))
    {
        addr_in = (struct sockaddr_in *)&addr;
        if (addr.sa_family == AF_INET)
        {
            char *ip = (char *)evm_malloc(INET_ADDRSTRLEN);
            memset(ip, 0, INET_ADDRSTRLEN);
            inet_ntop(addr_in->sin_family, &addr_in->sin_addr, ip, INET_ADDRSTRLEN);
            evm_prop_append(e, result, "address", evm_mk_heap_string((intptr_t)ip));
            evm_prop_append(e, result, "post", evm_mk_number(ntohs(addr_in->sin_port)));
            evm_prop_append(e, result, "family", evm_mk_foreign_string("IPv4"));
        }
        else
        {
            char *ip = (char *)evm_malloc(INET6_ADDRSTRLEN);
            memset(ip, 0, INET6_ADDRSTRLEN);
            inet_ntop(addr_in->sin_family, &addr_in->sin_addr, ip, INET6_ADDRSTRLEN);
            evm_prop_append(e, result, "address", evm_mk_heap_string((intptr_t)ip));
            evm_prop_append(e, result, "post", evm_mk_number(ntohs(addr_in->sin_port)));
            evm_prop_append(e, result, "family", evm_mk_foreign_string("IPv6"));
        }
    }

    evm_module_event_emit(e, p, "listening", 0, NULL);

    return *result;
}

//dgram.Socket.bind([port][, address][, callback])
//dgram.Socket.bind(options[, callback]) not support
//emit event of listening
static evm_val_t evm_module_udp_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    struct sockaddr_in laddr;
    memset(&laddr, 0, sizeof(struct sockaddr_in));
    laddr.sin_family = PF_INET;
    laddr.sin_port = htons(IPERF_PORT_LOCAL);
    laddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (argc > 0 && evm_is_number(v))
        laddr.sin_port = htons(evm_2_integer(v));

    if (argc > 1 && evm_is_string(v + 1))
        laddr.sin_addr.s_addr = inet_addr(evm_2_string(v + 1));

    int ret = bind(*sockfd, (struct sockaddr *)&laddr, sizeof(laddr));
    if (ret < 0)
    {
        lwip_close(*sockfd);
        evm_free(sockfd);
        evm_set_err(e, ec_type, "bind error\n");
        return EVM_VAL_UNDEFINED;
    }

    //emit event of listening
    evm_module_event_emit(e, p, "listening", 0, NULL);

    if (argc > 2 && evm_is_script(v + 2))
        evm_run_callback(e, &e->scope, v + 2, NULL, 0);

    taskENTER_CRITICAL();
    xTaskCreate(_udp_recv_thread,  /* 任务函数名 */
                "udp-task-thread", /* 任务名，字符串形式，方便调试 */
                512,               /* 栈大小，单位为字，即4个字节 */
                sockfd,            /* 任务形参 void * */
                13,                /* 优先级，数值越大，优先级越高 */
                NULL);             /* 任务句柄 */
    taskEXIT_CRITICAL();

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.close([callback])
static evm_val_t evm_module_udp_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    int result = lwip_close(*sockfd);
    if (result < 0)
        evm_set_err(e, ec_type, "close socket failed\n");

    if (argc > 0 && evm_is_script(v))
    {
        evm_run_callback(e, &e->scope, v, NULL, 0);
    }

    //emit close event
    evm_module_event_emit(e, p, "close", 0, NULL);
    evm_free(sockfd);

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.send(msg, [offset, length,] port [, address] [, callback])
static evm_val_t evm_module_udp_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 5 || !evm_is_string(v) || !evm_is_buffer(v) || !evm_is_integer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_string(v + 4))
    {
        return EVM_VAL_UNDEFINED;
    }

    int port = evm_2_integer(v + 3);
    char *ip = evm_2_string(v + 4);

    /*complete the struct: sockaddr_in*/
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    /* send the string to server*/
    char *msg = NULL;
    int len = 0;
    if (evm_is_string(v))
    {
        msg = (intptr_t)evm_2_string(v);
        len = evm_string_len(v);
    }
    else
    {
        msg = evm_buffer_addr(v);
        len = evm_buffer_len(v);
    }

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    if (sendto(*sockfd, msg, len, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        evm_set_err(e, ec_type, "sendto error\n");
    }

    if (argc == 6 && evm_is_script(v + 5))
        evm_run_callback(e, &e->scope, v + 5, NULL, 0);

    return EVM_VAL_UNDEFINED;
}

//dgram.Socket.setBroadcast(flag)
static evm_val_t evm_module_udp_setBroadcast(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_boolean(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    int so_broadcast = 0;

    if (evm_2_boolean(v))
        so_broadcast = 1;

    int *sockfd = (int *)evm_object_get_ext_data(p);
    if (sockfd == NULL || sockfd == 0)
        return EVM_VAL_UNDEFINED;

    int result = setsockopt(*sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
    if (result < 0)
    {
        evm_set_err(e, ec_type, "set broadcast failed\n");
    }

    return EVM_VAL_UNDEFINED;
}

//dgram.createSocket(options[, callback])
//dgram.createSocket(type[, callback])
static evm_val_t evm_module_udp_createSocket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v) || !evm_is_object(v))
    {
        return EVM_VAL_UNDEFINED;
    }

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
    }

    int sockfd = socket(domain, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd < 0)
    {
        return EVM_VAL_UNDEFINED;
    }

    int one = 1;
    if (reuseAddr && setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
    {
        return EVM_VAL_UNDEFINED;
    }

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

    intptr_t *sock = evm_malloc(sizeof(sockfd));
    if (sock == NULL || sock == 0)
    {
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }

    *sock = sockfd;

    evm_object_set_ext_data(obj, sock);

    return *obj;
}

evm_err_t evm_module_udp(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"createSocket", evm_mk_native((intptr_t)evm_module_udp_createSocket)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "udp", builtin);
    return e->err;
}

#endif
