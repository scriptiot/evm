router = require("@system.router")

export default {
    data: {
        title: 'World',
    },

    onInit: function () {
        print("==========onInit==========")
    },

    onReady: function () {
        print("==========onReady==========")
    },

    onShow: function () {
        print("==========onShow==========")
    },

    onHide: function () {
        print("==========onHide==========")
    },

    onDestroy: function () {
        print("==========onDestroy==========")
    },

    onPageRedirect: function (obj) {
        print("==========onclick==========")
        print(obj.attributes.url)

        router.push({
            path: obj.attributes.url
        })
    },
}