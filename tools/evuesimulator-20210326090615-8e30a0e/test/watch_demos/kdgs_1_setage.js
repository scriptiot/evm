router = require("@system.router")

export default {
    data: {
        title: 'Hello,Developer!',
        age:5
    },
    onInit: function() {
        print("==========onInit index==========")
    },

    onReady: function() {
        print("==========onReady index==========")
    },

    onShow: function() {
        print("==========onShow index==========")
    },

    onHide: function() {
        print("==========onHide index==========")
    },

    onDestroy: function() {
        print("==========onDestroy index==========")
    },
    changeAge(obj){
        if(obj.attributes.id=='jia'){
            if(this.age!=50){
                this.age=this.age+1;
            }
        }else{
            if(this.age!=0){
                this.age=this.age-1;
            }
        }

    },
    toIndex(obj){
        router.push({
            path: obj.attributes.url
        })
    }

}