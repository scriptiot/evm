var BufferModule = require("buffer");
var Buffer = BufferModule.Buffer;

// Creates a zero-filled Buffer of length 10.
var buf1 = new Buffer(20);
print(buf1, buf1.toString())

// Creates a Buffer containing [0x1, 0x2, 0x3].
// var buf2 = new Buffer([65, 66, 67]);
// print(buf2.toString(), buf2)

// Creates a Buffer containing UTF-8 bytes [0x74, 0xc3, 0xa9, 0x73, 0x74].
// var buf3 = new Buffer('656667', 'hex');
// print(buf3, buf3.toString(), buf3.length)

// print(BufferModule.byteLength('4142'));

// var buf4 = BufferModule.from('4142','hex');

// prints: AB
// print(buf4.toString());

// var buf5 = new Buffer([65, 2 + 65, 65 - 3, 65]);

// // prints: AAAA
// console.log(buf5, buf5.length);

// var buf6 = new Buffer("Hello world");

// // prints: Hello world
// console.log(buf6, buf6.toString(), buf6.length);

// var buf = BufferModule.concat([ new Buffer('He'),
//                              new Buffer('llo'),
//                              new Buffer(' wo'),
//                              new Buffer('rld') ])
// // prints: Hello world
// console.log(buf, buf.toString());

// print(buf1.write("vw"))
// print(buf1)
// buf1.writeUInt8(65);
// print(buf1.toString(), buf1)
// print(buf1.fill(30))
// print(buf1.slice(3, 7))

var buf7 = new Buffer([1, 2, 3]);
var buf8 = new Buffer([1, 2, 5]);

// print(buf7.compare(buf8))

// buf7.copy(buf1)
// print(buf1, buf7)

print(buf7.equals(buf8))
print(buf1.writeUInt8(34, 7))
print(buf1)
print(buf1.writeUInt16LE(57005, 2))
print(buf1)
print(buf1.writeUInt32LE(0x41424344, 10))
print(buf1.readInt8(2))
print(buf1.readUInt8(8))