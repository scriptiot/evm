print('test -- plusminus');

assert((1 + 2) == 3);
assert((2 + 1) == 3);
assert((2 + 1) != 4);

assert((7 + 7) == 14);
assert((7 - 7) == 0);
assert((7 + 7) == 14);


var number = 81;
assert((number + 9) == 90);
assert((number - 9) == 72);

var num1 = 1234567, num2 = 1234000;
assert((num1 % num2) == 567);