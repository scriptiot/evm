router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!',
    },
    toSearch(obj){
        router.push({
            path: obj.attributes.url
        })
    }
}