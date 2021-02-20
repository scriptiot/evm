var Buffer = require("buffer");

// Creates a zero-filled Buffer of length 10.
// var buf1 = new Buffer(10);
// print(buf1)

// Creates a Buffer containing [0x1, 0x2, 0x3].
// var buf2 = new Buffer([1, 2, 3]);
// print(buf2.toString())

// Creates a Buffer containing UTF-8 bytes [0x74, 0xc3, 0xa9, 0x73, 0x74].
var buf3 = new Buffer('tést');
print(buf3, buf3.length)

var buffer = new Buffer([65, 256 + 65, 65 - 256, 65.1]);

// prints: AAAA
console.log(buffer, buffer.length);

print(Buffer.byteLength('4142'));

// var buffer = Buffer.from('4142','hex');

// //prints: AB
// print(buffer.toString());

// var buf = new Buffer(4);

// buf.write('w');
// buf.writeUInt8(65);

// print(buf.toString(), buf)

// buf.fill(30)

// print(buf)