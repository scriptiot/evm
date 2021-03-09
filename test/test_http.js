var http = require('http');

var data_A = 'Data to upload..';
var data_B = 'more data';

print(data_A.length)
print(data_B.length)

http.get({
    port: 8888,
    host: "http://192.168.31.226",
    path: "/",
    headers: {},
}, function (response) {
    print('Got response');
    if (response) response.on('data', function (chunk) {
        print('Chunk: ');
        print(chunk);
    });
});

// var request = http.request({
//     port: 8888,
//     host: "http://192.168.31.226",
//     path: "/",
//     method: "POST",
//     headers: { 'Content-Length': data_A.length + data_B.length }
// }, function (response) {
//     print('Got response');
//     if (response) response.on('data', function (chunk) {
//         print('Chunk: ');
//         print(chunk);
//     });
// });

// request.write(data_A);
// request.write(data_B);
// request.end();