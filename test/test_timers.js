var timer = setInterval(function () {
    print('hello,world')
}, 500);
setTimeout(function () {
    print("123213")
    print(timer)
    clearInterval(timer);
 }, 2000)