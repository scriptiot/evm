router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!',
        keyword:"孙悟空",
    },
    onReturn(){
        router.pash({
            path:"kdgs_1_search3",
            params:{
                keyword:this.data.keyword,
            }

        })
    },
    toSearch(obj){
        router.push({
            path: obj.attributes.url
        })
    }
}