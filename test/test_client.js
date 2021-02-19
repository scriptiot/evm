net = require('net');
var socket = net.createConnection(8000, '127.0.0.1', function() {
    socket.write('hello evm')
});

socket.on('data', function(data) {
    print(data);
});