print('Hello EVM');
wlan = require('wlan');
console.log(wlan);
console.log(wlan.connect);

wlan.connect('edmund_m', '11112222');

timer = require('timer');

timer.mdelay(1000 * 10);
print('socket starts to send data');

socket = require('socket');
tcp = socket.tcp();
print('tcp');
tcp.host(tcp, "192.168.31.5");
print('host');
tcp.port(tcp, 8080);
print('port');
tcp.start(tcp);
print('start');

while(true) {
    timer.mdelay(1000 * 3);
    print('send');
    tcp.send(tcp, "hello evm");
}
