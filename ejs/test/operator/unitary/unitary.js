print('test -- unitary');
var o = new Object;
o.name = "David";
print(o.name);	//输出 "David"
delete o.name;
print(o.name);	//输出 "undefined"

var iNum1 = 2;
var iNum2 = 20;
var iNum3 = --iNum1 + ++iNum2;	//等于 "22"
var iNum4 = iNum1 + iNum2;
print(iNum1)
print(iNum2)
