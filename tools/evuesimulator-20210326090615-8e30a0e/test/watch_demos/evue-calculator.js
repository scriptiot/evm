router = require("@system.router")

export default {
    data: {
        title: 'World',
        result: "",
    },
    onclick: function() {
        this.data.title = 'hello';
        print("=====onclick=====");
        show();
        gc();
    },
    onlongpress: function() {
        print("=====longpress=====");
    },
    onReturn: function() {
        print("=====onReturn=====");
        router.push({
            path: "evue-dock"
        })
    },
    concatString: function(obj) {
        print(obj.attributes.value)
        this.result = this.result + obj.attributes.value
        print(this.result)
    },
    clear: function() {
        this.result = ""
    },
    getResult: function() {
        if (this.result != "") {
            this.result = eval(this.result)
        }
    }
}