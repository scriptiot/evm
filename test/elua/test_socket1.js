socket = require('socket');
rtos = require('rtos');
tcp = socket.tcp();
tcp.host(tcp, "192.168.31.229");
tcp.port(tcp, 8080);
tcp.on(tcp, "connect", function(id, re){
    tcp.send(tcp, "EVM is connected to server");
});

tcp.on(tcp, "recv", function(id, data) {
    console.log(data);
});

tcp.start(tcp);
print('start');