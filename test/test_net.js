var net = require("net");

var port = 7878;
var host = "192.168.31.226";
var echo_msg = "";

var socket = net.connect(port, host, function () {
  socket.end("Hello IoT.js");
});

socket.on("data", function (data) {
  echo_msg += data;
});

socket.on("end", function () {
  console.log(echo_msg);
});
