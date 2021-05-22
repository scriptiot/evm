router = require("@system.router")

export default {
    data: {
        title: 'World',
        currentIndex: 0,
        currentImg0: { display: 'flex' },
        currentImg1: { display: 'none' },
        currentImg2: { display: 'none' },
    },

    onShow: function() {
        // this.currentIndex = this.currentIndex % 3
        // for (var i = 0; i < 3; i++) {
        //     this.data['currentImg' + i] = { display: 'none' }
        // }
        // this.data['currentImg' + this.currentIndex] = { display: 'flex' }
        // this.currentIndex = this.currentIndex + 1
    },

    onClose: function() {
        print("=====on close modal=====");
        // this.modalStyle = { display: 'none' }
        router.push({ path: "evue-dock" })
    },
    onlongpress: function() {
        print("=====longpress=====");
    },

    onButtonPress: function() {
        print("=====onButtonPress=====");
    },

    onButtonLongPress: function() {
        print("=====onButtonLongPress=====");
    },

    onButtonRelease: function() {
        print("=====onButtonRelease=====");
    },

    onButtonClick: function(obj) {
        print(obj.attributes)
        this.currentIndex = this.currentIndex % 3
        for (var i = 0; i < 3; i++) {
            this.data['currentImg' + i] = { display: 'none' }
        }
        this.data['currentImg' + this.currentIndex] = { display: 'flex' }
        this.currentIndex = this.currentIndex + 1
    },
}