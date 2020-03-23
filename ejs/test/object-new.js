print('test -- Object -- new');

var a = [1,2,3]
var b = new Object(a)
b[0] = 10

print(a)
print(b)

assert(a === b)
