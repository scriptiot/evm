// Copyright JS Foundation and other contributors, http://js.foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

var console = require('console');
print('test -- object-defineProperty');
// 使用 __proto__
var obj = {};
var descriptor = Object.create(null); // 没有继承的属性
// 默认没有 enumerable，没有 configurable，没有 writable
descriptor.value = 'static';
console.log(descriptor.__proto__)
Object.defineProperty(obj, 'key', descriptor);

Object.defineProperty(obj, "key1", {
  enumerable: false,
  configurable: false,
  writable: false,
  value: "static1"
});
obj.key1 = 1
assert(obj.hasOwnProperty('key') === true)
assert(obj.hasOwnProperty('key1') === true)
console.log(Object.keys(obj))
console.log(obj.__proto__)
console.log(obj.__proto__.isPrototypeOf(obj))

var obj = Object.create({'1':2,'b':4});
console.log(obj)
console.log(obj.__proto__)
console.log(Object.keys(obj))
console.log(obj.toString())

function Tree(name) {
   this.name = name;
}

var theTree = new Tree("Redwood");
assert( theTree.constructor === Tree);

var obj = {'a':1};

var c = Object.defineProperty (obj, "prop", {
    value: 2010,
    writable: true,
    enumerable: true,
    configurable: false
});
print(obj)
print(c)
assert (obj.hasOwnProperty ("prop"));

assert(obj.constructor === Object)

assert(Object.prototype.isPrototypeOf(Object.__proto__))

// function getFunc() {
//     return 20;
// }

// try {
//     Object.defineProperty (obj, "prop", {
//         get: getFunc
//     });
//     assert (false);
// } catch (e) {
//     assert (e instanceof TypeError);
//     var desc = Object.getOwnPropertyDescriptor (obj, "prop");
//     assert (desc.value === 2010);
//     assert (typeof (desc.get) === 'undefined');
// }

// obj = {};
// var setter = function () {};

// Object.defineProperty(obj, "prop", {
//     set: setter,
//     configurable: true
// });

// var desc1 = Object.getOwnPropertyDescriptor(obj, "prop");

// Object.defineProperty(obj, "prop", {
//     set: undefined
// });

// var desc2 = Object.getOwnPropertyDescriptor(obj, "prop");
// assert (desc1.set === setter && desc2.set === undefined);

// obj = {};

// /* This error is thrown even in non-strict mode. */
// Object.defineProperty(obj, 'f', {
//   set: function(value) { throw 234; },
// });

// try {
//   obj.f = 5;
//   assert (false);
// } catch (err) {
//   assert (err === 234);
// }

// try {
//   Object.defineProperty(42, "prop", {
//       set: undefined
//   });
//   assert (false);
// } catch (e) {
//   assert (e instanceof TypeError);
// }
