#ifdef CONFIG_EVM_MODULE_NET
#include "evm_module.h"
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "netdb.h"

#define _SOCKET_READ_BUF_SIZE   4096

typedef struct _net_sock_t {
    struct sockaddr_in addr;
    int sockfd;
    int obj_id;
    pthread_t pid;
    int alive;
    uint8_t rx_buf[_SOCKET_READ_BUF_SIZE];
} _net_sock_t;

static void _net_server_thread(_net_sock_t *server_sock) {
    _net_sock_t *client_sock;
    while(client_sock->alive) {
        client_sock = evm_malloc(sizeof(_net_sock_t));
        if( client_sock ) {
            client_sock->sockfd = accept(server_sock->sockfd, (struct sockaddr *)&client_sock->addr, sizeof(struct sockaddr_in));
            if (client_sock->sockfd < 0) {
                evm_print("accept connection failed!\r\n");
                evm_free(client_sock);
                continue;
            }
        }
    }
    evm_free(server_sock);
}

static void _net_client_thread(_net_sock_t *client_sock) {
    size_t bytes_read;
    while(client_sock->alive) {
        bytes_read = read(client_sock->sockfd, client_sock->rx_buf, _SOCKET_READ_BUF_SIZE);
        if( bytes_read > 0 && bytes_read < _SOCKET_READ_BUF_SIZE ) {
            evm_val_t *obj = evm_module_registry_get(evm_runtime, client_sock->obj_id);
            if( obj ) {
                evm_val_t *args = evm_buffer_create(evm_runtime, bytes_read);
                if( args ){
                    memcpy( evm_buffer_addr(args), client_sock->rx_buf, bytes_read);
                    evm_module_event_emit(evm_runtime, obj, "data", 1, args);
                    evm_pop(evm_runtime);
                }
            }
        } else if( bytes_read < 0 ){
            close(client_sock->sockfd);
            break;
        }
        usleep(1000);
    }
    evm_free(client_sock);
}

//server.close([closeListener])
static evm_val_t evm_module_net_server_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *server_sock = evm_object_get_ext_data(p);
    if( !server_sock )
        return EVM_VAL_UNDEFINED;
    close(server_sock->sockfd);
	return EVM_VAL_UNDEFINED;
}

//server.listen(port[, host][, backlog][, listenListener])
//server.listen(options[, listenListener])
static evm_val_t evm_module_net_server_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc < 1 || !evm_is_integer(v) ) {
        return EVM_VAL_UNDEFINED;
    }

    _net_sock_t *server_sock = evm_object_get_ext_data(p);
    if( !server_sock )
        return EVM_VAL_UNDEFINED;

    server_sock->addr.sin_family = AF_INET;
    server_sock->addr.sin_port = htons(evm_2_integer(v));
    server_sock->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(server_sock->sockfd, (struct sockaddr*)&server_sock->addr, sizeof(struct sockaddr_in)) < 0 )
    {
        evm_print("Failed to bind server socket\r\n");
        close(server_sock->sockfd);
        return EVM_VAL_UNDEFINED;
    }

    if( listen(server_sock->sockfd, 1) < 0 ) {
        evm_print("Failed to listen\r\n");
        close(server_sock->sockfd);
    }

    pthread_create(&server_sock->pid, NULL, _net_server_thread, server_sock);
	return EVM_VAL_UNDEFINED;
}

//server.on(event, callback)
static evm_val_t evm_module_net_server_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc < 2 || !evm_is_string(v) || !evm_is_script(v + 1) ) {
        return EVM_VAL_UNDEFINED;
    }

    _net_sock_t *server_sock = (_net_sock_t*)evm_object_get_ext_data(p);
    if( !server_sock )
        return EVM_VAL_UNDEFINED;
    
    if( server_sock->obj_id == -1 )
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
	return EVM_VAL_UNDEFINED;
}

//socket.connect(options[, connectListener])
//socket.connect(port[, host][, connectListener])
static evm_val_t evm_module_net_socket_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock = evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;

    if( argc > 2 && evm_is_integer(v) && evm_is_string(v + 1) ) {
        sock->addr.sin_family = AF_INET;
        sock->addr.sin_port = htons(evm_2_integer(v));
        sock->addr.sin_addr.s_addr = inet_addr(evm_2_string(v + 1));
        memset(&(sock->addr.sin_zero), 0, sizeof(sock->addr.sin_zero));
    } else {
        return EVM_VAL_UNDEFINED;
    }

    if ((sock->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        evm_print("Socket create failed.");
        return EVM_VAL_UNDEFINED;
    }

    if ( connect(sock->sockfd, (struct sockaddr *)&(sock->addr), sizeof(struct sockaddr)) == -1 ) {
        evm_print("socket connect failed!");
        close(sock->sockfd);
        return EVM_VAL_UNDEFINED;
    }

    if( sock->obj_id != -1 ){
        evm_module_event_add_listener(e, p, "connect", v + 2);
        evm_module_event_emit(e, p, "connect", 0, NULL);
    }

    pthread_create(&sock->pid, NULL, _net_client_thread, sock);

	return EVM_VAL_UNDEFINED;
}

//socket.destroy()
static evm_val_t evm_module_net_socket_destroy(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_net_sock_t *sock = evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;
    close(sock->sockfd);
    sock->alive = 0;
    evm_module_registry_remove(e, sock->obj_id);
    return EVM_VAL_UNDEFINED;
}

//socket.end([data][, callback])
static evm_val_t evm_module_net_socket_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock = evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;
    if( argc > 0 ) {
        if( evm_is_string(v) ) {
            send(sock->sockfd, evm_2_string(v), evm_string_len(v), 0);
        } else if( evm_is_buffer(v) ) {
            send(sock->sockfd, evm_buffer_addr(v), evm_buffer_len(v), 0);
        }
    }

    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_run_callback(e, v + 2, NULL, NULL, 0);
    }

    shutdown(sock->sockfd, SHUT_WR);
	return EVM_VAL_UNDEFINED;
}

//socket.pause()
static evm_val_t evm_module_net_socket_pause(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.resume()
static evm_val_t evm_module_net_socket_resume(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.setKeepAlive([enable][, initialDelay])
static evm_val_t evm_module_net_socket_setKeepAlive(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock = evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;
    if( argc > 0 && evm_is_boolean(v) ) {
        if( evm_is_true(v) ) {
            int keepAlive = 1;
            setsockopt(sock->sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
        } else {
            int keepAlive = 0;
            setsockopt(sock->sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
        }
    }
	return EVM_VAL_UNDEFINED;
}

//socket.setTimeout(timeout[, callback])
static evm_val_t evm_module_net_socket_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.write(data[, callback])
static evm_val_t evm_module_net_socket_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock = evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;
    if( argc > 0 ) {
        if( evm_is_string(v) ) {
            send(sock->sockfd, evm_2_string(v), evm_string_len(v), 0);
        } else if( evm_is_buffer(v) ) {
            send(sock->sockfd, evm_buffer_addr(v), evm_buffer_len(v), 0);
        }
    }
	return EVM_VAL_UNDEFINED;
}

//socket.on(event, callback)
static evm_val_t evm_module_net_socket_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock = (_net_sock_t *)evm_object_get_ext_data(p);
    if( !sock )
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
	return EVM_VAL_UNDEFINED;
}

//net.createConnection(options[, connectListener])
//net.createConnection(port[, host][, connectListener])
static evm_val_t evm_module_net_createConnection(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _net_sock_t *sock;
    evm_val_t *obj = evm_object_create(e, GC_DICT, 9, 0);
    if( !obj )
        return EVM_VAL_UNDEFINED;
    evm_prop_append(e, obj, "connect", evm_mk_native((intptr_t)evm_module_net_socket_connect));
    evm_prop_append(e, obj, "destroy", evm_mk_native((intptr_t)evm_module_net_socket_destroy));
    evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_net_socket_end));
    evm_prop_append(e, obj, "pause", evm_mk_native((intptr_t)evm_module_net_socket_pause));
    evm_prop_append(e, obj, "resume", evm_mk_native((intptr_t)evm_module_net_socket_resume));
    evm_prop_append(e, obj, "setKeepAlive", evm_mk_native((intptr_t)evm_module_net_socket_setKeepAlive));
    evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_net_socket_setTimeout));
    evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_net_socket_write));
    evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_net_socket_on));

    sock = evm_malloc(sizeof(_net_sock_t));
    if( !sock ) {
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }

    evm_object_set_ext_data(obj, (intptr_t)sock);

    sock->alive = 1;
    sock->obj_id = evm_module_registry_add(e, obj);

    evm_module_net_socket_connect(e, obj, argc, v);
    return *obj;
}

//net.connect(options[, connectListener])
//net.connect(port[, host][, connectListener])
static evm_val_t evm_module_net_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_net_createConnection(e, p, argc, v);
}

//net.createServer([options][, connectionListener])
static evm_val_t evm_module_net_createServer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *server_obj = NULL;
    _net_sock_t *server_sock;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0 ) {
        evm_print("Failed to create socket\r\n");
        return EVM_VAL_UNDEFINED;
    }
    
    server_obj = evm_object_create(e, GC_OBJECT, 3, 0);
    if( !server_obj ) {
        close(sockfd);
        return EVM_VAL_UNDEFINED;
    }
    evm_prop_append(e, server_obj, "close", evm_mk_native((intptr_t)evm_module_net_server_close));
    evm_prop_append(e, server_obj, "listen", evm_mk_native((intptr_t)evm_module_net_server_listen));
    evm_prop_append(e, server_obj, "on", evm_mk_native((intptr_t)evm_module_net_server_on));

    server_sock = evm_malloc(sizeof(_net_sock_t));
    if( !server_sock ) {
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }
    server_sock->sockfd = sockfd;
    server_sock->alive = 1;
    evm_object_set_ext_data(server_obj, (intptr_t)server_sock);

    server_sock->obj_id = evm_module_registry_add(e, server_obj);
    return *server_obj;
}

//new net.Socket([options])
static evm_val_t evm_module_net_socket_new(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t *obj = evm_object_create(e, GC_DICT, 9, 0);
    if( obj ) {
        evm_prop_append(e, obj, "connect", evm_mk_native((intptr_t)evm_module_net_socket_connect));
        evm_prop_append(e, obj, "destroy", evm_mk_native((intptr_t)evm_module_net_socket_destroy));
        evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_net_socket_end));
        evm_prop_append(e, obj, "pause", evm_mk_native((intptr_t)evm_module_net_socket_pause));
        evm_prop_append(e, obj, "resume", evm_mk_native((intptr_t)evm_module_net_socket_resume));
        evm_prop_append(e, obj, "setKeepAlive", evm_mk_native((intptr_t)evm_module_net_socket_setKeepAlive));
        evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_net_socket_setTimeout));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_net_socket_write));
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_net_socket_on));

        _net_sock_t *sock = evm_malloc(sizeof(_net_sock_t));
        if( sock ) {
            sock->alive = 1;
            evm_object_set_ext_data(obj, (intptr_t)sock);
        }
        return *obj;
    }
	return EVM_VAL_UNDEFINED;
}

evm_val_t *_net_socket_create(evm_t *e) {
    _net_sock_t *sock;
    evm_val_t *obj = evm_object_create(e, GC_DICT, 9, 0);
    if( !obj )
        return NULL;
    evm_prop_append(e, obj, "connect", evm_mk_native((intptr_t)evm_module_net_socket_connect));
    evm_prop_append(e, obj, "destroy", evm_mk_native((intptr_t)evm_module_net_socket_destroy));
    evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_net_socket_end));
    evm_prop_append(e, obj, "pause", evm_mk_native((intptr_t)evm_module_net_socket_pause));
    evm_prop_append(e, obj, "resume", evm_mk_native((intptr_t)evm_module_net_socket_resume));
    evm_prop_append(e, obj, "setKeepAlive", evm_mk_native((intptr_t)evm_module_net_socket_setKeepAlive));
    evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_net_socket_setTimeout));
    evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_net_socket_write));
    evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_net_socket_on));

    sock = evm_malloc(sizeof(_net_sock_t));
    if( !sock ) {
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }

    evm_object_set_ext_data(obj, (intptr_t)sock);
    sock->alive = 1;
    sock->obj_id = evm_module_registry_add(e, obj);
}

static evm_object_native_t _net_socket_native = {
    .creator = evm_module_net_socket_new,
};

evm_err_t evm_module_net(evm_t *e) {
    evm_val_t *socket = evm_native_function_create(e, &_net_socket_native, 0);
    if( !socket )
        return e->err;

	evm_builtin_t builtin[] = {
		{"connect", evm_mk_native((intptr_t)evm_module_net_connect)},
        {"createConnection", evm_mk_native((intptr_t)evm_module_net_createConnection)},
        {"createServer", evm_mk_native((intptr_t)evm_module_net_createServer)},
        {"Socket", *socket},
        {NULL, EVM_VAL_UNDEFINED}
	};
    evm_module_create(e, "net", builtin);
    evm_pop(e);
	return e->err;
}
#endif
