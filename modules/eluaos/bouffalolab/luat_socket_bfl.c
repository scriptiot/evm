
#include "luat_socket.h"
#include "luat_msgbus.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>

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

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("Connect fail!\n");
        goto __exit;
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
        closesocket(sock);
    return 0;
}

int luat_socket_is_ready(void) {
    return 1;
}

uint32_t luat_socket_selfip(void) {
    uint32_t ip, gw, mask;
    wifi_mgmr_sta_ip_get(&ip, &gw, &mask);
    return ip;
}

