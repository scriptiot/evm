var net = require('net');

var port = 8000;

var server = net.createServer(
  {
    allowHalfOpen: true
  },
  function(socket) {
    server.close();
  }
);

server.listen(port);