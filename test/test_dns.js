var dns = require('dns');
dns.lookup('www.baidu.com', function(err, ip, family) {
    print(ip);
});