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
            router.push({ path: "evue-dock" })
        }, 200)
    },

    onHide: function() {
        print("==========onHide index==========")
    },

    onDestroy: function() {
        print("==========onDestroy index==========")
    },
}