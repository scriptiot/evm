import socket

udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# AF_INET 表示使用IPv4, SOCK_DGRAM 则表明数据将是数据报(datagrams)，
# 对应 TCP 的是 SOCK_STREAM 表明数据对象是字节流(stream)
udp.bind(('127.0.0.1', 4567))

while 1:
    data, addr = udp.recvfrom(1024)  # 返回数据以及发送数据的地址
    print(data.decode('utf-8'))
    udp.sendto('server accept: '.encode('utf-8') + data, addr)  # 数据和对应的地址
udp.close()
