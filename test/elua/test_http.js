// http example
wlan = require('wlan');
timer = require('timer');
rtos = require('rtos');
http = require('http');

var ssid = 'your ssid'
var pwd = 'your pwd'

wlan.connect(ssid, pwd);
timer.mdelay(1000 * 10);

setInterval(function() {
    rtos.receive(10);
}, 10);

http.get("http://store.evmiot.com/api/v1/evm_store/evm", null, function(code, headers, body) {
    print("http", code, headers, body)
});

http.post("http://store.evmiot.com/api/v1/evm_store/evm", {

    }, 
    function(code, headers, body) {
        print("http", code, headers, body)
    }
);
