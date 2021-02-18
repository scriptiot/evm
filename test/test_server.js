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

server.on('connection', function(socket) {
  var msg = '';
  socket.on('data', function(data) {
    msg += data;
  });
  socket.on('end', function() {
    socket.end(msg);
  });
});