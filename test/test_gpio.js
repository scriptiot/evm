var gpio = require('gpio');

// var gpio5 = gpio.open({
//     pin: 5,
//     direction: gpio.DIRECTION.OUT,
//     mode: gpio.MODE.PUSHPULL,
//     edge: gpio.EDGE.RISING
// }, function (err, pin) {
//     print(pin)
//     print(err)
//     print(pin.readSync())
// });

var gpio5 = gpio.openSync({
    pin: 5,
    direction: gpio.DIRECTION.OUT,
    mode: gpio.MODE.NONE
});

var value = gpio5.readSync();
print(value);

setInterval(function () {
    gpio5.writeSync(value ? 0 : 1);
    value = !value;
}, 500)