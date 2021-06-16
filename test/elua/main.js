print('Welcome to ELUAOS');

rtos = require('rtos');

setInterval(function() {
    rtos.receive(10);
}, 10);

//example codes
require('test_socket1.js')