net = require('net');
var socket = net.createConnection(8000, '192.168.1.2', function() {
    socket.end('Hello IoT.js');
    print("send end");
});