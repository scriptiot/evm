router = require("@system.router")

export default {
    data: {
        title: 'World'
    },
    onclick: function () {
        this.data.title = 'hello';
        print("=====onclick=====");
        show();
        gc();
    },
    onlongpress: function () {
        print("=====longpress=====");
    },
    onReturn: function () {
        print("=====onReturn=====");
        router.push({ path: "album" })
    }
}