print('Hello EVM');

wlan = require('wlan');
timer = require('timer');
rtos = require('rtos');
socket = require('socket');

var ssid = 'bytecode'
var pwd = 'bytecode888'

wlan.connect(ssid, pwd);
timer.mdelay(1000 * 10);

//socket.isReady
print("socket.isReady");
console.log(socket.isReady());

//socket.ip
print("socket.ip");
console.log(socket.ip());

//socket.tsend
socket.tsend("192.168.31.229", 8080, "hello evm");

tcp = socket.tcp();
tcp.host(tcp, "192.168.31.229");
tcp.port(tcp, 8080);
tcp.on(tcp, "connect", function(id, re){
    print("so:sockid: ", tcp.sockid(tcp));
    tcp.send(tcp, "EVM is connected to server");
});

tcp.on(tcp, "recv", function(id, data) {
    console.log(data);
    print("tcp.close");
    tcp.close(tcp)
    print("tcp.clean");
    tcp.clean(tcp)
});

print('start');
tcp.start(tcp);

setInterval(function() {
    rtos.receive(10);
}, 10);

show();
gc();
show();
