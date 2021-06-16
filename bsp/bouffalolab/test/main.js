print('Hello EVM');

wlan = require('wlan');
timer = require('timer');
rtos = require('rtos');

console.log(wlan);
console.log(wlan.connect);

wlan.connect('bytecode', 'bytecode888');

timer.mdelay(1000 * 10);

socket = require('socket');
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

setInterval(function() {
    rtos.receive(10);
}, 10);

gc();
show();
