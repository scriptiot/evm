print('test -- break');
var iNum = 0;

for (var i=1; i<10; i++) {
  if (i % 5 == 0) {
    break;
  }
  iNum++;
}
print(iNum);	//输出 "4"
