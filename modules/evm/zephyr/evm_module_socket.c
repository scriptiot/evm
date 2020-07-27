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
 

typedef struct socket_obj_t {
    int fd;
    int af;
    int type;
    int proto;
}socket_obj_t;

evm_val_t * evm_module_socket_object_create(evm_t* e);

// typedef struct _getaddrinfo_state_t {
//     evm_val_t result;
//     struct k_sem sem; // zephyr 系统信号量 猜测包含在<zephyr.h>里面
//     evm_val_t port;
//     int status;
// } getaddrinfo_state_t;

// socket(af=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP)
static evm_val_t evm_module_socket(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int af = AF_INET, type = SOCK_STREAM, proto =IPPROTO_TCP;
    if(argc>0) af = evm_2_integer(v);
    if(argc>1) type = evm_2_integer(v+1);
    if(argc>2) proto = evm_2_integer(v+2);

    socket_obj_t *socket = (socket_obj_t*)malloc(sizeof(socket_obj_t));
    socket->proto = proto;
    socket->type = type;
    socket->af = af;

    socket->fd = zsock_socket(af,type,proto);
    evm_object_set_ext_data(p, (intptr_t)socket);

    return EVM_VAL_UNDEFINED;
}


//close()
static evm_val_t evm_module_socket_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);
    int res = zsock_close(socket->fd);
    return EVM_VAL_UNDEFINED;
}


// bind(ip, port)
static evm_val_t evm_module_socket_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc > 1 && evm_is_string(v) && evm_is_number(v + 1))
    {
        socket_obj_t *socket = (socket_obj_t*)evm_object_get_ext_data(p);
        char * ip_str = evm_2_string(v);
        int port = evm_2_integer(v + 1);

        struct sockaddr_in addr4;
        memset(&addr4, 0, sizeof(addr4));
        addr4.sin_family = AF_INET;
        addr4.sin_port = htons(port);
        zsock_inet_pton(AF_INET, ip_str, &addr4.sin_addr);

        int ret = zsock_bind( socket->fd, &addr4, sizeof(addr4) );
        if (ret < 0) {
            evm_print("Failed to bind socket\r\n");
            return EVM_VAL_FALSE;
        }
        evm_print("Successfully bind socket\r\n");
        return EVM_VAL_TRUE;
    }
    return EVM_VAL_UNDEFINED;
}

// connect(address)
static evm_val_t evm_module_socket_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=1)
    {
        evm_val_t address = evm_mk_list(v);
        int len = evm_list_len(&address);
        if(len!=2)
            EVM_ARG_LENGTH_ERR

        char* ip_addr = evm_2_string(evm_list_get(e,&address,0));
        char* port = evm_2_string(evm_list_get(e,&address,1));

        struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);

        struct zsock_addrinfo hints, *addr_list, *cur;

        /* Do name resolution with both IPv6 and IPv4 */
        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = socket->type ;
        hints.ai_protocol = socket->proto;

        if( zsock_getaddrinfo( ip_addr, port, &hints, &addr_list ) != 0 )
            return EVM_VAL_NULL;

        struct sockaddr sockaddr;

        for( cur = addr_list; cur != NULL; cur = cur->ai_next )
        {
            if( zsock_connect( socket->fd, cur->ai_addr, cur->ai_addrlen ) != 0 )
            {
                zsock_close( socket->fd );
            }
            break;
        }
    }else{
        EVM_ARG_LENGTH_ERR
    }

    return EVM_VAL_UNDEFINED;
}

// listen([backlog])
static evm_val_t evm_module_socket_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int backlog = 2;
    if(argc >0) backlog = evm_2_integer(v);
    backlog = backlog>0 ? backlog:0;

    struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);
    
    int res = zsock_listen(socket->fd, backlog);

    return EVM_VAL_UNDEFINED;
}

// accept()
static evm_val_t evm_module_socket_accept(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    socket_obj_t *socket = (socket_obj_t*)evm_object_get_ext_data(p);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int ctx = zsock_accept(socket->fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (ctx < 0) {
		evm_print("accept error");
		return EVM_VAL_FALSE;
	}

    // int port = ntohs(client_addr.sin_port);

	// char ip_str[INET_ADDRSTRLEN];   

	// inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));

    // evm_val_t *address = evm_list_create(e,GC_LIST,2);
    // evm_val_t *ip_val = evm_heap_string_create(e, ip_str, sizeof(ip_str));
    // evm_list_set(e, address, 0, *ip_val);
    // evm_list_set(e, address, 1, evm_mk_number(port));

    evm_val_t * obj = evm_module_socket_object_create(e);

    int af = client_addr.sin_family, type = socket->type, proto =socket->proto;

    socket_obj_t *new_socket = (socket_obj_t*)malloc(sizeof(socket_obj_t));
    new_socket->proto = proto;
    new_socket->type = type;
    new_socket->af = af;
    new_socket->fd = ctx;
    evm_object_set_ext_data(obj, (intptr_t)new_socket);

    return *obj;

}

// send(bytes|string)
static evm_val_t evm_module_socket_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc > 0 && ( evm_is_string(v) || evm_is_buffer(v) ) ){
        struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);
        char * bytes;
        int len;
        if( evm_is_string(v) ){
            bytes = evm_2_string(v);
            len = evm_string_len(v);
        } else {
            bytes = evm_buffer_addr(v);
            len = evm_buffer_len(v);
        }
        len = zsock_send(socket->fd, bytes, len, 0);
        return evm_mk_number(len);
    }
    return EVM_VAL_UNDEFINED;
}

// sendall(bytes)

// recv(bufsize)
static evm_val_t evm_module_socket_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=1) EVM_ARG_LENGTH_ERR
    int bufsize = evm_2_integer(v);

    struct socket_obj_t * socket = (struct socket_obj_t *)evm_object_get_ext_data(p);
    evm_val_t *buf = evm_buffer_create(e, bufsize);
    int recv_len = zsock_recv(socket->fd, buf, bufsize, 0);

    return evm_mk_number(recv_len) ;
}

// sendto(bytes, address)
static evm_val_t evm_module_socket_sendto(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc !=2) EVM_ARG_LENGTH_ERR
    evm_val_t *bytes = evm_mk_buffer(v);
    char *buf = evm_buffer_addr(bytes);
    int len = evm_buffer_len(buf);

    evm_val_t address = evm_mk_list(v);
    int lenaddr = evm_list_len(&address);
    if(len!=2)
        EVM_ARG_LENGTH_ERR

    char* ip_addr = evm_2_string(evm_list_get(e,&address,0));
    char* port = evm_2_string(evm_list_get(e,&address,1));

    struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);

    struct zsock_addrinfo hints, *addr_list, *cur;
    /* Do name resolution with both IPv6 and IPv4 */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socket->type ;
    hints.ai_protocol = socket->proto;

    if( zsock_getaddrinfo( ip_addr, port, &hints, &addr_list ) != 0 )
        return EVM_VAL_NULL;

    struct sockaddr sockaddr;
    for( cur = addr_list; cur != NULL; cur = cur->ai_next )
    {
        if(zsock_sendto(socket->fd, buf, len, 0, cur->ai_addr, cur->ai_addrlen ) != 0 )
        {
            zsock_close( socket->fd );
        }
        break;
    }
    
    return EVM_VAL_UNDEFINED;
}

// recvfrom(bufsize)
static evm_val_t evm_module_socket_recvfrom(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=1) EVM_ARG_LENGTH_ERR
    int max_len = evm_2_integer(v);
    if(max_len<0) 
        return EVM_VAL_UNDEFINED;

    struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);


    struct sockaddr sock_addr;
    socklen_t addrlen = sizeof(sock_addr);

    char *buf = (char *)malloc(max_len);
    int rec_cnt = zsock_recvfrom(socket->fd, buf, max_len, 0, &sock_addr, addrlen);
    if(rec_cnt==-1) {
        free(buf);
        return EVM_VAL_NULL;
    }
    evm_val_t *buffer = evm_buffer_create(e,rec_cnt);
    evm_buffer_set(buffer,buf,0,rec_cnt);

    struct sockaddr_in *sock = ( struct sockaddr_in*)&sock_addr;
    int port = ntohs(sock->sin_port);
	struct in_addr in  = sock->sin_addr;
	char ip_str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
	//成功的话此时IP地址保存在str字符串中。
	inet_ntop(AF_INET,&in, ip_str, sizeof(ip_str));
    evm_val_t *address = evm_list_create(e,GC_LIST,2);
    evm_list_push(e,address,1,evm_heap_string_create(e,ip_str,sizeof(ip_str)));
    evm_list_push(e,address,1,evm_mk_number(port));

    evm_val_t *res = evm_list_create(e,GC_LIST,2);
    evm_list_push(e,res,1,buffer);
    evm_list_push(e,res,1,address);

    free(buf);
    return *res;

}

// setsockopt(level, optname, value)
static evm_val_t evm_module_socket_setsockopt(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=3) EVM_ARG_LENGTH_ERR
    int level = evm_2_integer(v);
    int optname = evm_2_integer(v+1);
    evm_val_t *value = evm_mk_buffer(v+2);
    int optlen = evm_buffer_len(value);
    char *optval = evm_buffer_addr(value);

    struct socket_obj_t *socket = (struct socket_obj_t*)evm_object_get_ext_data(p);
    zsock_setsockopt(socket->fd, level, optname, optval, optlen);
    return EVM_VAL_UNDEFINED;
}


// settimeout(value)
// setblocking(flag)
// makefile(mode='rb', buffering=0)
// read([size])
// readinto(buf[, nbytes])
// readline()
// write(buf)

//getaddrinfo(host, port, af=0, type=0, proto=0, flags=0)
//return (family, type, proto, canonname, sockaddr)
static evm_val_t evm_module_socket_getaddrinfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc<2)
        EVM_ARG_LENGTH_ERR
    char *host = evm_2_string(v);
    char* port = evm_2_string(v+1);
    int af = 0,type=0,proto=0,flags=0;
    if(argc>2) af = evm_2_integer(v+2);
    if(argc>3) type = evm_2_integer(v+3);
    if(argc>4) proto = evm_2_integer(v+4);
    if(argc>5) flags = evm_2_integer(v+5);

    struct zsock_addrinfo hints, *addr_list, *aip;

    /* Do name resolution with both IPv6 and IPv4 */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_protocol = proto;
    hints.ai_flags = flags;
    hints.ai_family= af;

    if( zsock_getaddrinfo( host, port, &hints, &addr_list ) != 0 )
        return EVM_VAL_NULL;

        /* 显示获取的信息 */

    evm_val_t *address_list = evm_list_create(e,GC_LIST,5);
	for (aip = addr_list; aip != NULL; aip = aip->ai_next)
	{ 
        struct sockaddr_in *sinp; 
		sinp = (struct sockaddr_in *)aip->ai_addr;

        char buf[INET_ADDRSTRLEN]; 
        char *addr = inet_ntop(AF_INET, &sinp->sin_addr, buf, INET_ADDRSTRLEN); 
		printf(" addr = %s, port = %d\n", addr?addr:"unknow ", ntohs(sinp->sin_port)); 

        evm_val_t *sockaddr = evm_list_create(e,GC_LIST,2);
        evm_val_t *str_addr = evm_heap_string_create(e,addr,strlen(addr));
        evm_val_t port = evm_mk_number(ntohs(sinp->sin_port));

        evm_list_push(e,&sockaddr,1,str_addr);
        evm_list_push(e,&sockaddr,1,&port);

        evm_val_t *canonname = evm_heap_string_create(e,aip->ai_canonname,strlen(aip->ai_canonname));

        evm_list_push(e,address_list,1,evm_mk_number(addr_list->ai_family));
        evm_list_push(e,address_list,1,evm_mk_number(addr_list->ai_socktype));
        evm_list_push(e,address_list,1,evm_mk_number(addr_list->ai_protocol));
        evm_list_push(e,address_list,1,canonname);
        evm_list_push(e,address_list,1,sockaddr);
        break;
	}

    return address_list;
}


// inet_pton(af, txt_addr) 
static evm_val_t evm_module_socket_inet_pton(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=2) EVM_ARG_LENGTH_ERR
    int af = evm_2_integer(v);
    char *txt_addr = evm_2_string(v+1);
    struct in_addr dst;
    zsock_inet_pton(af, txt_addr, &dst);
    // return EVM_VAL_UNDEFINED;
    evm_val_t *buffer = evm_buffer_create(e,4);
    evm_buffer_set(buffer,dst.s4_addr,0,4);
    return *buffer;
}
// inet_ntop(af, bin_addr)
static evm_val_t evm_module_socket_inet_ntop(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if(argc!=2) EVM_ARG_LENGTH_ERR
    int af = evm_2_integer(v);
    evm_val_t *bin_addr_buffer = evm_mk_buffer(v+1);
    char *bin_addr = evm_buffer_addr(bin_addr_buffer);

    struct in_addr src;
    memcpy(&src.s4_addr,bin_addr,4);

    char dst[16]={0};
    zsock_inet_ntop(af, &src, dst, 16);
    evm_val_t *str_addr = evm_heap_string_create(e,dst,16);
    return *str_addr;
}

evm_val_t * evm_module_socket_object_create(evm_t* e){
    evm_val_t * obj = evm_object_create(e, GC_OBJECT, 20, 0);
    int i = 0;
    evm_prop_set(e, obj, i++, "connect", evm_mk_native((intptr_t)evm_module_socket_connect));
    evm_prop_set(e, obj, i++, "close", evm_mk_native((intptr_t)evm_module_socket_close));
    evm_prop_set(e, obj, i++, "bind", evm_mk_native((intptr_t)evm_module_socket_bind));
    evm_prop_set(e, obj, i++, "connect", evm_mk_native((intptr_t)evm_module_socket_connect));
    evm_prop_set(e, obj, i++, "listen", evm_mk_native((intptr_t)evm_module_socket_listen));
    evm_prop_set(e, obj, i++, "accept", evm_mk_native((intptr_t)evm_module_socket_accept));
    evm_prop_set(e, obj, i++, "recv", evm_mk_native((intptr_t)evm_module_socket_recv));
    evm_prop_set(e, obj, i++, "send", evm_mk_native((intptr_t)evm_module_socket_send));
    evm_prop_set(e, obj, i++, "sendto", evm_mk_native((intptr_t)evm_module_socket_sendto));
    evm_prop_set(e, obj, i++, "recvfrom", evm_mk_native((intptr_t)evm_module_socket_recvfrom));
    evm_prop_set(e, obj, i++, "setsockopt", evm_mk_native((intptr_t)evm_module_socket_setsockopt));
    evm_prop_set(e, obj, i++, "getaddrinfo", evm_mk_native((intptr_t)evm_module_socket_getaddrinfo));
    evm_prop_set(e, obj, i++, "inet_pton", evm_mk_native((intptr_t)evm_module_socket_inet_pton));       
    evm_prop_set(e, obj, i++, "inet_ntop", evm_mk_native((intptr_t)evm_module_socket_inet_ntop));
    evm_prop_set(e, obj, i++, "AF_INET", evm_mk_number(AF_INET));
    evm_prop_set(e, obj, i++, "AF_INET6", evm_mk_number(AF_INET6));
    evm_prop_set(e, obj, i++, "SOCK_STREAM", evm_mk_number(SOCK_STREAM));
    evm_prop_set(e, obj, i++, "SOCK_DGRAM", evm_mk_number(SOCK_DGRAM));
    evm_prop_set(e, obj, i++, "IPPROTO_UDP", evm_mk_number(IPPROTO_UDP));
    evm_prop_set(e, obj, i++, "IPPROTO_TCP", evm_mk_number(IPPROTO_TCP));
    return obj;
}

evm_val_t evm_class_socket(evm_t *e)
{
    evm_builtin_t class_socket[] = {
        {"connect", evm_mk_native((intptr_t)evm_module_socket_connect)},
        {"close", evm_mk_native((intptr_t)evm_module_socket_close)},
        {"bind", evm_mk_native((intptr_t)evm_module_socket_bind)},
        {"connect", evm_mk_native((intptr_t)evm_module_socket_connect)},
        {"listen", evm_mk_native((intptr_t)evm_module_socket_listen)},
        {"accept", evm_mk_native((intptr_t)evm_module_socket_accept)},
        {"recv", evm_mk_native((intptr_t)evm_module_socket_recv)},
        {"send", evm_mk_native((intptr_t)evm_module_socket_send)},
        {"sendto", evm_mk_native((intptr_t)evm_module_socket_sendto)},
        {"recvfrom", evm_mk_native((intptr_t)evm_module_socket_recvfrom)},
        {"setsockopt", evm_mk_native((intptr_t)evm_module_socket_setsockopt)},
        {"getaddrinfo", evm_mk_native((intptr_t)evm_module_socket_getaddrinfo)},
        {"inet_pton", evm_mk_native((intptr_t)evm_module_socket_inet_pton)},       
        {"inet_ntop", evm_mk_native((intptr_t)evm_module_socket_inet_ntop)},
        
        {"AF_INET", evm_mk_number(AF_INET)},
        {"AF_INET6", evm_mk_number(AF_INET6)},

        {"SOCK_STREAM", evm_mk_number(SOCK_STREAM)},
        {"SOCK_DGRAM", evm_mk_number(SOCK_DGRAM)},

        {"IPPROTO_UDP", evm_mk_number(IPPROTO_UDP)},
        {"IPPROTO_TCP", evm_mk_number(IPPROTO_TCP)},

        {NULL, NULL}};
    return *evm_class_create(e, (evm_native_fn)evm_module_socket, class_socket, NULL);
}
