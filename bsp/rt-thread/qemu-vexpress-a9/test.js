uart = require('uart');
var config = {
    device: 'uart0',
    baudRate: 115200,
    dataBits: 8
}

uart0 = uart.openSync(config);
uart0.writeSync('hello evm');
