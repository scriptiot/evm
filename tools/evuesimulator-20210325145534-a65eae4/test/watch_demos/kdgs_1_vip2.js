router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!',
        adminName:'Marvin',
        date:'开通VIP，畅听10万+精品内容',
    },
    onPageRedirect: function (obj) {
        print("==========onclick==========")
        print(obj.attributes.url)

        router.push({
            path: obj.attributes.url
        })
    },
}