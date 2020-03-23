var console = require('console');
// console.log = print;

console.log('test -- Object');

var a = [1,2,3];
var b = new Object(a);
b[0] = 10;

console.log(a);
console.log(b);

assert (a === b);

console.log(new Object([1,2,3]))
console.log(new Object(new Array(1,2,3)))

assert(typeof new Object('new 123Array(1,2,3))') === "string");


// var a = Object
// assert (a.length === 1)
var b = {"b1": 1, "b2": 2}
var c = Object.create(b)
console.log(c)
console.log(b)
console.log(c.b1)
assert (c.b1 === b.b1)
assert (c.b2 === b.b2)
c.b1 = 10
console.log(c)
console.log(b)


var person = {
    isHuman: 122,
};
console.log(person.isHuman)
var me = Object.create(person);
me.name = "Matthew"; // "name" is a property set on "me", but not on "person"
me.isHuman = true; // inherited properties can be overwritten
console.log(person); // 此处有问题，isHuman返回true了
console.log(me)




var arr = ['a', 'b', 'c'];
console.log(arr)
console.log(Object.keys(arr)); // console: ['0', '1', '2']
console.log(typeof Object.keys(arr)[1]); // console: ['0', '1', '2']

// array like object
var obj = { '4': 'a', '1': 'b', '2': 'c' };
keys = Object.keys(obj); // console: ['0', '1', '2']
values = Object.values(obj);

console.log(keys)
console.log(values)



var obj = { foo: 'bar', baz: 42 };
console.log(Object.values(obj)); // ['bar', 42]

// // array like object
// var obj = { 0: 'a', 1: 'b', 2: 'c' };
// console.log(obj);
// console.log(Object.values(obj)); // ['a', 'b', 'c']

// // // array like object with random key ordering
// // // when we use numeric keys, the value returned in a numerical order according to the keys
// var an_obj = { 100: 'a', 2: 'b', 7: 'c' };
// console.log(Object.values(an_obj)); // ['b', 'c', 'a']

// getFoo is property which isn't enumerable
var my_obj = Object.create({}, { getFoo: { value: function() { return this.foo; } } });
my_obj.foo = 'bar';
console.log(Object.values(my_obj)); // ['bar']

// non-object argument will be coerced to an object
assert(Object.keys('foo') === ['f', 'o', 'o']); // ['f', 'o', 'o']
assert(Object.keys(123) === []); // []
console.log(Object.values('foo')); // ['f', 'o', 'o']
assert (Object.values(123) === []); // []

//不可枚举属性
var my_obj = Object.create({}, {
  getFoo: {
    value: function() { return this.foo; },
    enumerable: true
  }
});
my_obj.foo = 1;

console.log(Object.keys(my_obj))
console.log(Object.values(my_obj))
console.log(my_obj.getFoo())
