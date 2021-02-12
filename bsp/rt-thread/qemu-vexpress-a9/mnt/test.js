uart = require('uart');
var config = {
    device: 'uart0',
    baudRate: 115200,
    dataBits: 8
}

uart0 = uart.openSync(config);
uart0.writeSync('');

gpio = require('gpio');
print(gpio.DIRECTION.IN);
print(gpio.MODE.PULLUP);
print(gpio.EDGE.FALLING);
config = {
    pin: 0,
    direction: gpio.DIRECTION.IN,
    mode: gpio.MODE.FLOAT
}
pin = gpio.openSync(config);
pin.writeSync(1);
