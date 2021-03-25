router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!'
    },
    onInit: function() {
        print("==========onInit index==========")
    },

    onReady: function() {
        print("==========onReady index==========")
    },

    onShow: function() {
        print("==========onShow index==========")
        setTimeout(function() {
            print("==========xxxxxxxxxxxxxxxxxxx==========")
            router.push({ path: "kdgs_1_setage" })
        }, 500)
    },

    onHide: function() {
        print("==========onHide index==========")
    },

    onDestroy: function() {
        print("==========onDestroy index==========")
    },
}