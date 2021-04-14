// var dgram = require('dgram');
// var server = dgram.createSocket('udp4');

// var BufferModule = require("buffer");
// var Buffer = BufferModule.Buffer;

// server.on('error', function (err) {
//     console.log('Error: ' + err);
//     server.close();
// });

// server.on('message', function (data, rinfo) {
//     // prints: message received
//     console.log('server got: ');

//     var buf2 = new Buffer(data);
//     console.log(buf2.toString(), buf2);
//     console.log(rinfo);

//     // server.send("hello,world", 0, 11, rinfo.port, rinfo.address, function () {
//     //     console.log("complete!!!");
//     // });
// });

// server.on('listening', function () {
//     console.log('server listening at ' + server.address().port);
// });

// server.bind(4567);

var dgram = require('dgram');
var socket = dgram.createSocket("udp4");

socket.send('Hello IoT.js', 0, 12, 4567, "127.0.0.1", function () {
    console.log("complete!!!")
});