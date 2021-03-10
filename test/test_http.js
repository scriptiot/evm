var http = require("http");
var BufferModule = require("buffer");
var Buffer = BufferModule.Buffer;

var data_A = "Data to upload..";
var data_B = "more data";

print(data_A.length);
print(data_B.length);

// var request = http.request(
//   {
//     port: 7777,
//     host: "http://192.168.31.20",
//     path: "/",
//     method: "GET",
//     headers: {},
//   },
//   function (response) {
//     print("Got response");
//     print(response);
//     if (response)
//       response.on("data", function (chunk) {
//         var buf2 = new Buffer(chunk);
//         print(buf2.toString());
//         print("come in.......");
//         print(chunk);
//       });
//   }
// );

var request = http.get({
  port: 7777,
  host: "http://192.168.31.20",
  path: "/",
  method: "GET",
  headers: {},
});

request.on("response", function (response) {
  print("Got response:");
  print(response);

  if (response)
    response.on("data", function (chunk) {
      var buf2 = new Buffer(chunk);
      print(buf2.toString());
      print("come in.......");
      print(chunk);
    });
});

request.end(132, function () {
  print("execute here!!!");
});
