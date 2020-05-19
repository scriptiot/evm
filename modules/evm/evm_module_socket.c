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

#include <net/socket.h>

/**
 * @brief SOCKET class constructor
 * 
 * @param name device name
 * 
 * @uasge new SOCKET() 
 */
static evm_val_t evm_module_socket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_socket(evm_t *e)
{
	evm_builtin_t class_socket[] = {
		{"socket", evm_mk_native((intptr_t)evm_module_socket_socket)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_class_socket, class_socket, NULL);
}

static evm_val_t evm_module_socket_socket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_socket(family, type, proto);
}

static evm_val_t evm_module_socket_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_close(sock);
}

static evm_val_t evm_module_socket_shutdown(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_shutdown(sock, how);
}

static evm_val_t evm_module_socket_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_bind(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_connect(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_listen(sock, backlog);
}

static evm_val_t evm_module_socket_accept(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_accept(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_send(sock, buf, len, flags);
}

static evm_val_t evm_module_socket_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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
	//return zsock_setsockopt(sock, level, optname, optval, optlen);
}

static evm_val_t evm_module_socket_getsockname(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	//return zsock_getsockname(sock, addr, addrlen);
}

static evm_val_t evm_module_socket_getaddrinfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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


