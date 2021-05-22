router = require("@system.router")

export default {
    data: {
        title: 'World',
        increased: true,
        opacity: 0.1,
        list_style:{
            display:'show'
        },
        img_style:{
            display:'none'
        },
        text_style:{
            display:'none'
        },
        json: [{
            name: '三只小白兔',
            icon: 'kdgs_1_gl1.bin',
            isVip: '1',
            id: '123456',
        }, {
            name: '美人鱼故事',
            icon: 'kdgs_1_gl2.bin',
            isVip: '0',
            id: '123457',
        }],
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

    onclick: function () {
        if (this.style['background-color'] != 'transparent') {
            this.style['background-color'] = 'transparent';
        } else {
            this.style['background-color'] = 'red';
        }

        // this.opacity += 0.1;
        // this.style['border-opacity'] = this.opacity
        // this.style['opacity'] = this.opacity

        if (this.increased && this.style['border-width'] < 120) {
            print("=====onclick==+++++++++++++++===", this.style['border-width']);
            this.style['border-width'] += 10;
            this.style['opacity'] += 0.1;
            this.style['border-opacity'] += 0.1
            print("=====onclick==+++++++++++++++===", this.style['border-width']);
        } else {
            this.increased = false;
        }

        if (!this.increased && this.style['border-width'] > 0) {
            this.style['border-width'] -= 10;
            this.style['opacity'] -= 0.1;
            this.style['border-opacity'] -= 0.1
            print("=====onclick==888888888888888888===", this.style['border-width']);
        } else {
            this.increased = true;
        }

        var _b = Number(this.style['border-width'])
        this.title = this.style['border-width'].toString()
        print("=====onclick==xxxxxxxxxxxxxxxxx===");
        // print(this);
        print("=====onclick=====");
    },
    onlongpress: function () {
        print("=====longpress=====");
    },

    onButtonPress: function () {
        print("=====onButtonPress=====");
    },

    onButtonLongPress: function () {
        print("=====onButtonLongPress=====");
    },

    onPageRedirect: function (obj, x, y) {
        print("==========onclick==========")
        print(obj.attributes.url)
        router.push({
            path: obj.attributes.url
        })
    },

    onButtonRelease: function () {
        print("=====onButtonRelease=====");
    },

    onButtonClick: function () {
        print("=====onButtonClick=====");
        print(this.$page)
        show();
    },

    onReturn: function () {
        print("=====onReturn=====");
        router.push({
            path: "evue-dock",
            params: {
                a: 1,
                b: 2
            }
        })
    },

    toPlayPage: function (obj) {
        router.push({
            path: "kdgs_1_storyPlay",
            params: {
                a: 1,
                b: 2
            }
        })
    }
}