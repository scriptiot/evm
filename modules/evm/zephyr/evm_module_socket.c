/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/


#include "evm_module.h"

#include <zephyr.h>
#include <net/net_context.h>
#include <net/net_pkt.h>
#include <net/dns_resolve.h>
#include <net/socket.h>

#define STATE_NEW 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2
#define STATE_PEER_CLOSED 3

typedef struct _socket_obj_t {
    int ctx;
    char state;
} socket_obj_t;

typedef struct _getaddrinfo_state_t {
    evm_val_t result;
    struct k_sem sem; // zephyr 系统信号量 猜测包含在<zephyr.h>里面
    evm_val_t port;
    int status;
} getaddrinfo_state_t;

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));return evm_mk_undefined(); }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));return evm_mk_undefined(); }
#define RAISE_ERRNO(x) { int _err = x; if (_err < 0) { evm_set_err(e, ec_type, "Socket error");evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));return EVM_VAL_UNDEFINED; } }
#define RAISE_SOCK_ERRNO(x) { if ((int)(x) == -1) { evm_set_err(e, ec_type, "Socket failed");evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));return EVM_VAL_UNDEFINED; } }

/**
 * @brief SOCKET class constructor
 * 
 * @param name device name
 * 
 * @uasge new SOCKET() 
 */

socket_obj_t *socket_new(void) {
    socket_obj_t *socket = (socket_obj_t *)malloc(sizeof(socket_obj_t));
    // socket->base.type = (mp_obj_t)&socket_type; // bu zhi dao shen me gui
    socket->state = STATE_NEW;
    return socket;
}

static evm_val_t evm_module_socket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t *socket = socket_new();
    evm_object_set_ext_data(p, (intptr_t)socket);
    return EVM_VAL_UNDEFINED;
}

static void parse_inet_addr(socket_obj_t *socket, evm_val_t *v, struct sockaddr *sockaddr) {
    struct sockaddr_in *sockaddr_in = (struct sockaddr_in *)sockaddr;

    sockaddr_in->sin_family = net_context_get_family((void *)socket->ctx);
    int _err = net_addr_pton(sockaddr_in->sin_family, evm_2_string(v), &sockaddr_in->sin_addr);
    RAISE_ERRNO(_err)
    sockaddr_in->sin_port = htons(evm_2_integer(v + 1));
}

static evm_val_t evm_module_socket_socket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int family = AF_INET;
    int socktype = SOCK_STREAM;
    int proto = -1;

    if (argc >= 1) {
        family = evm_2_integer(v);
        if (argc >= 2) {
            socktype = evm_2_integer(v + 1);
            if (argc >= 3) {
                proto = evm_2_integer(v + 2);
            }
        }
    }

    if (proto == -1) {
        proto = IPPROTO_TCP;
        if (socktype != SOCK_STREAM) {
            proto = IPPROTO_UDP;
        }
    }

    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);

    socket->ctx = zsock_socket(family, socktype, proto);
    RAISE_SOCK_ERRNO(socket->ctx)

    return EVM_VAL_UNDEFINED;
    //return zsock_socket(family, type, proto);
}

static evm_val_t evm_module_socket_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    int res = zsock_close(socket->ctx);
    RAISE_SOCK_ERRNO(socket->ctx)

    evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));
    return EVM_VAL_UNDEFINED;
    //return zsock_close(sock);
}

static evm_val_t evm_module_socket_shutdown(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_shutdown(sock, how);
}

static evm_val_t evm_module_socket_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    struct sockaddr sockaddr;
    parse_inet_addr(socket, v, &sockaddr);

    int res = zsock_bind(socket->ctx, &sockaddr, sizeof(sockaddr));
    RAISE_ERRNO(res)

    return EVM_VAL_NULL;
    //return zsock_bind(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    struct sockaddr sockaddr;
    parse_inet_addr(socket, v, &sockaddr);

    int res = zsock_connect(socket->ctx, &sockaddr, sizeof(sockaddr));
    RAISE_ERRNO(res)

    return EVM_VAL_NULL;
    //return zsock_connect(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    int backlog = evm_2_integer(v);
    int res = zsock_listen(socket->ctx, backlog);
    RAISE_SOCK_ERRNO(res)

    return EVM_VAL_NULL;
    //return zsock_listen(sock, backlog);
}

static evm_val_t evm_module_socket_accept(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    struct sockaddr sockaddr;
    socklen_t addrlen = sizeof(sockaddr);

    socket_obj_t *socket2 = socket_new();
    socket2->ctx = zsock_accept(socket->ctx, &sockaddr, &addrlen);

    evm_val_t *client = evm_list_create(e, GC_TUPLE, 2);

    // 这里如何释放socket2S申请的内存???
    evm_list_set(e, client, 0, evm_mk_native((intptr_t)socket2));
    evm_list_set(e, client, 1, evm_mk_null());

    return *client;
    //return zsock_accept(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    int size = evm_buffer_len(v);

    evm_val_t *buf = evm_buffer_create(e, size);

    int len = zsock_send(socket->ctx, buf, size, 0);
    RAISE_SOCK_ERRNO(len)

    return evm_mk_number(len);
    //return zsock_send(sock, buf, len, flags);
}

static evm_val_t evm_module_socket_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t * socket = (socket_obj_t *)evm_object_get_ext_data(p);
    RAISE_SOCK_ERRNO(socket->ctx)

    int max_len = evm_2_integer(v);
    // +1 to accommodate for trailing \0
    evm_val_t *buf = evm_buffer_create(evm_t *e, max_len + 1);

    int recv_len = zsock_recv(socket->ctx, buf, max_len, 0);
    RAISE_SOCK_ERRNO(recv_len)

    return *buf;
    //return zsock_recv(sock, buf, max_len, flags);
}

/* This conflicts with fcntl.h, so code must include fcntl.h before socket.h: */
//#define fcntl zsock_fcntl

static evm_val_t evm_module_socket_sendto(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_sendto(sock, buf, len, flags, dest_addr, addrlen);
}

static evm_val_t evm_module_socket_sendmsg(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_sendmsg(sock, message, flags);
}

static evm_val_t evm_module_socket_recvfrom(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_recvfrom(sock, buf, max_len, flags, src_addr, addrlen);
}

static evm_val_t evm_module_socket_poll(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_poll(fds, nfds, timeout);
}

static evm_val_t evm_module_socket_getsockopt(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_getsockopt(sock, level, optname, optval, optlen);
}

static evm_val_t evm_module_socket_setsockopt(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    (void)argc; // always 4
    return EVM_VAL_NULL;
    //return zsock_setsockopt(sock, level, optname, optval, optlen);
}

static evm_val_t evm_module_socket_getsockname(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_getsockname(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_getaddrinfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t host_in = v, port_in = v + 1;
    const char *host = evm_2_string(host_in);
    int family = 0;
    if (argc > 2) {
        family = evm_2_integer(v + 2);
    }

    getaddrinfo_state_t state;

    if (!evm_is_integer(port_in)) ARG_TYPE_ERR
    state.port = port_in;
    state.result = evm_list_create(e, GC_LIST, 0);
    k_sem_init(&state.sem, 0, UINT_MAX);

    for (int i = 2; i--;) {
        int type = (family != AF_INET6 ? DNS_QUERY_TYPE_A : DNS_QUERY_TYPE_AAAA);
        RAISE_ERRNO(dns_get_addr_info(host, type, NULL, dns_resolve_cb, &state, 3000));
        k_sem_take(&state.sem, K_FOREVER);
        if (family != 0) break;
        family = AF_INET6;
    }

    // Raise error only if there's nothing to return, otherwise
    // it may be IPv4 vs IPv6 differences.
    int len = evm_list_len(state.result);
    if (state.status != 0 && len == 0) {
        evm_set_err(e, ec_type, "Nothing return");
        evm_object_set_destroy(p, (evm_native_fn)evm_object_get_ext_data(p));
        return evm_mk_undefined();
    }

    return state.result;
    //return zsock_getaddrinfo(host, service, hints, res);
}

static void evm_module_socket_freeaddrinfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //zsock_freeaddrinfo(ai);
}

static evm_val_t *evm_module_socket_gai_strerror(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_gai_strerror(errcode);
}

static evm_val_t evm_module_socket_getnameinfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
}

//#define addrinfo zsock_addrinfo

static evm_val_t evm_module_socket_gethostname(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_gethostname(buf, len);
}

static evm_val_t evm_module_socket_inet_pton(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_inet_pton(family, src, dst);
}

static evm_val_t *evm_module_socket_inet_ntop(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //return zsock_inet_ntop(family, src, dst, size);
}

evm_val_t evm_class_socket(evm_t *e)
{
    evm_builtin_t class_socket[] = {
        {"socket", evm_mk_native((intptr_t)evm_module_socket_socket)},
        {NULL, NULL}};
    return *evm_class_create(e, (evm_native_fn)evm_class_socket, class_socket, NULL);
}
