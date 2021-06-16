
#include "luat_socket.h"
#include "luat_msgbus.h"

#if 1

#include <sys/socket.h> 
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>

#define SAL_TLS_HOST    "site0.cn"
#define SAL_TLS_PORT    80
#define SAL_TLS_BUFSZ   1024

#define DBG_TAG           "luat.socket"
#define DBG_LVL           DBG_INFO

#if 1

static int socket_ntp_handler(lua_State *L, void* ptr) {
    return 1;
}
static void ntp_thread(void* params) {

}

int luat_socket_ntp_sync(const char* hostname) {

    return 1;
}
#else
int l_socket_ntp_sync(const char* hostname) {
    return -1;
}
#endif
int luat_socket_tsend(const char* hostname, int port, void* buff, int len)
{
    int ret, i;
    // char *recv_data;
    struct hostent *host;
    int sock = -1, bytes_received;
    struct sockaddr_in server_addr;

    // 强制GC一次先
    //lua_gc(L, LUA_GCCOLLECT, 0);

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(hostname);

    /* 创建一个socket，类型是SOCKET_STREAM，TCP 协议, TLS 类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket error\n");
        goto __exit;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    while(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1 && errno != EISCONN)
    {
        if ( errno != EINTR ){
            printf("Connect fail!\n");
            perror("");
            goto __exit;
        }
    }

    /* 发送数据到 socket 连接 */
    ret = send(sock, buff, len, 0);
    if (ret <= 0)
    {
        printf("send error,close the socket.\n");
        goto __exit;
    }

__exit:
    // if (recv_data)
    //     rt_free(recv_data);

    if (sock >= 0)
        close(sock);
    return 0;
}

#include <arpa/inet.h>         /* 包含 ip_addr_t 等地址相关的头文件 */

int luat_socket_is_ready(void) {
    return 1;
}

uint32_t luat_socket_selfip(void) {
    return 0;
}

#endif
