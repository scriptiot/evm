var net = require("net");
var BufferModule = require("buffer");
var Buffer = BufferModule.Buffer;

var port = 8085;
var host = "192.168.31.226";
var echo_msg = "msg:";

var socket = net.connect(port, host, function () {
  socket.end("Hello IoT.js");
});

socket.on("data", function (data) {
    print(data);

    var buf2 = new Buffer(data);
    print(buf2.toString(), buf2);

    echo_msg += buf2.toString();
});

socket.on("end", function () {
  console.log(echo_msg);
});
