print('test -- casting');

var sColor = "red";
print(sColor.length);	//输出 "3"

var iNum = 10;
print(iNum.toString(2));	//输出 "1010"
print(iNum.toString(8));	//输出 "12"
print(iNum.toString(16));	//输出 "A"

var iNum1 = parseInt("12345red");	//返回 12345
print(iNum1)
var iNum1 = parseInt("0xA");	//返回 10
print(iNum1)
var iNum1 = parseInt("56.9");	//返回 56
print(iNum1)
var iNum1 = parseInt("red");	//返回 NaN
print(iNum1)

var iNum1 = parseInt("010");	//返回 8
print(iNum1)
var iNum2 = parseInt("010", 8);	//返回 8
print(iNum2)
var iNum3 = parseInt("010", 10);	//返回 10
print(iNum3)


var fNum1 = parseFloat("12345red");	//返回 12345
print(fNum1)
var fNum2 = parseFloat("0xA");	//返回 NaN
print(fNum2)
var fNum3 = parseFloat("11.2");	//返回 11.2
print(fNum3)
var fNum4 = parseFloat("11.22.33");	//返回 11.22
print(fNum4)
var fNum5 = parseFloat("0102");	//返回 102
print(fNum5)
var fNum1 = parseFloat("red");	//返回 NaN
print(fNum1)


var b1 = Boolean("");		//false - 空字符串
var b2 = Boolean("hello");		//true - 非空字符串
var b1 = Boolean(50);		//true - 非零数字
var b1 = Boolean(null);		//false - null
var b1 = Boolean(0);		//false - 零
var b1 = Boolean(new object());	//true - 对象


var num1 = Number(false)	//0
var num1 = Number(true)	//1
var num1 = Number(undefined)	//NaN
var num1 = Number(null)	//0
var num1 = Number("1.2")	//1.2
var num1 = Number("12")	//12
var num1 = Number("1.2.3")	//NaN
var num1 = Number(new object())	//NaN
var num1 = Number(50) //50

var s1 = String(null);	//"null"
var oNull = null;
var s2 = oNull.toString();	//会引发错误
