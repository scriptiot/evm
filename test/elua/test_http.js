http = require('http');

http.get("http://store.evmiot.com/api/v1/evm_store/evm", null, function(ret, code, headers, body) {
    print("http", ret, code, headers, body)
}) 
