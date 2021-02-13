net = require('net');
socket = new net.Socket();
print(socket);
socket.connect(8000, '192.168.1.2', function() {
    print(socket);
    socket.end('Hello IoT.js');
    print('send data');
});

socket.destroy();