# coding:utf-8

import signal
import socket
import time
import threading


class GracefulKiller:
    kill_now = False

    def __init__(self):
        signal.signal(signal.SIGINT, self.exit_gracefully)
        signal.signal(signal.SIGTERM, self.exit_gracefully)

    def exit_gracefully(self, signum, frame):
        self.kill_now = True


# 这里需要修改为你电脑局域网IP地址
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("127.0.0.1", 12345))

client.send(b"hello,world")


def main():
    killer = GracefulKiller()

    while True:
        streambytes = client.recv(1024)
        data = streambytes.decode('utf-8')
        if not data:
            client.close()
            break
        print(data)
        time.sleep(1)
        client.send(streambytes)

        if killer.kill_now:
            client.close()
            break


if __name__ == "__main__":
    main()
