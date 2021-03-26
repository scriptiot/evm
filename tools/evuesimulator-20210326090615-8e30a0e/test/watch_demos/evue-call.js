router = require("@system.router")

export default {
    data: {
        title: 'World'
    },
    onInit: function () {
        print("==========onInit progress==========")
    },

    onReady: function () {
        print("==========onReady progress==========")
    },

    onShow: function () {
        print("==========onShow progress==========")
    },

    onHide: function () {
        print("==========onHide progress==========")
    },

    onDestroy: function () {
        print("==========onDestroy progress==========")
    },

    onReturn: function () {
        router.push({ path: "evue-dock" })
    }
}