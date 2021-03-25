router = require("@system.router")

export default {
    data: { words: '有什么可以帮您？' },
    onclick: function () {
        this.words = 'hello';
        print("=====onclick=====");
    },
    onlongpress: function () {
        print("=====longpress=====");
    },
    onReturn: function () {
        print("=====onReturn=====");
        router.push({
            path: "evue-dock"
        })
    }
}