print('test -- continue');
var iNum = 0;

for (var i=1; i<10; i++) {
  print(i)
  if (i % 5 == 0) {
    continue;
  }
  iNum++;
}
print(iNum);	//输出 "8"
