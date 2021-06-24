// socket example
timer = require('timer');
rtos = require('rtos');
socket = require('socket');

var ssid = 'your ssid';
var pwd = 'your pwd';
var ip ='192.168.1.1';
var port = 8080;

wlan.connect(ssid, pwd);
timer.mdelay(1000 * 10);

setInterval(function() {
    rtos.receive(10);
}, 10);

//socket.isReady
print("socket.isReady");
console.log(socket.isReady());

//socket.ip
print("socket.ip");
console.log(socket.ip());

//socket.tsend
socket.tsend(ip, port, "hello evm");

tcp = socket.tcp();
tcp.host(tcp, ip);
tcp.port(tcp, port);
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
