import socket

udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# print(udp_client.proto)

while 1:
    msg = input("enter your msg>>:")
    udp_client.sendto(msg.encode('utf-8'), ('127.0.0.1', 4567))
    print(udp_client.recv(1024).decode('utf-8'))
