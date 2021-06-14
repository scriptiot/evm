print('Hello EVM');
wlan = require('wlan');
console.log(wlan);
console.log(wlan.connect);

wlan.connect('edmund_m', '11112222');

timer = require('timer');

timer.mdelay(1000 * 10);
print('socket starts to send data');

socket = require('socket');
while(true) {
    socket.tsend('192.168.31.5', 8080, 'this is EVM\n');
    timer.mdelay(1000);
}
