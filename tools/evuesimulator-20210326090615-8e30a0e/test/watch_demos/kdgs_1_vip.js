router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!',
        adminName:'Marvin',
        date:'VIP到期时间：2019/01/31',
    },
    onPageRedirect: function (obj) {
        print("==========onclick==========")
        print(obj.attributes.url)

        router.push({
            path: obj.attributes.url
        })
    },
}