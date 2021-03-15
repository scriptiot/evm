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


def connection_handler(client, address):
    print('Accept new connection from %s:%s...' % address)
    while True:
        if not client:
            break

        data = client.recv(1024)
        if not data:
            client.close()
            break

        if killer.kill_now:
            client.close()
            break

        client.send(data)
        print("message:", data.decode('utf-8').encode('utf-8'))
        time.sleep(1)


server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 12345))
server.listen(5)  # 只监听5个连接，不传值则监听任意多个连接

killer = GracefulKiller()


def main():
    while True:
        client, address = server.accept()
        t = threading.Thread(target=connection_handler, args=(client, address))
        t.start()

        if killer.kill_now:
            client.close()
            break


if __name__ == "__main__":
    main()
