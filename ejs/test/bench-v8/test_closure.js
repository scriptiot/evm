var log_str = "";

function log(str)
{
    print(str);
}

function f(a, b, c)
{
    var x = 10;
    print(a);
    
    function g(d) {
        function h() {
            print(d);
            print(x);
        }
        print(b);
        print(c);
        h();
    }
    g(4);
    return g;
}

var g1 = f(1, 2, 3);
g1(5);