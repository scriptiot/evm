router = require("@system.router")

export default {
    data: {
        title: '热门推荐',
        increased: true,
        opacity: 0.1,
        msgbox_style: {
            'display': 'none'
        },
        msgbox_style1: {
            'display': 'none'
        },
        messageTitle: "口袋故事",
        messageSubtitle: "确定下此应用?",
        json:[{
          name:'口袋故事1',
          icon:'image/kdgs.bin',
          status:'0',
          price:'2',
          id:'123456',
        },{
            name:'国学诗词2',
            icon:'image/gxsc.bin',
            status:'0',
            price:'2',
            id:'123457',
        },{
            name:'生活百科3',
            icon:'image/shbk.bin',
            status:'1',
            price:'',
            id:'123458',
        }],
        list: [{
            name: '',
            icon: '',
            status: '', //状态信息0：未购买并且应用是付费的  1：免费
            price: '', //应用价格
            id:''  //应用id
          }, {
            name: '',
            icon: '',
            status: '',
            price: '', 
            id:''  
          }, {
            name: '',
            icon: '',
            status: '',
            price: '', 
            id:''  
          }, {
            name: '',
            icon: '',
            status: '',
            price: '', 
            id:''  
          }, {
            name: '',
            icon: '',
            status: '',
            price: '', 
            id:''  
          }],
        isShow: [{
            'display': 'show'
          }, {
            'display': 'none'
          }, {
            'display': 'none'
          }, {
            'display': 'none'
          }, {
            'display': 'none'
          }],
    },

    onInit: function () {
        for(var i=0;i<this.json.length;i++){
            this.isShow[i].display='show'
            this.json[i].name=this.list[i].name
            this.json[i].icon=this.list[i].icon
            this.json[i].status=this.list[i].status
            this.json[i].price=this.list[i].price
            this.json[i].id=this.list[i].id
         }
        // this.messageTitle=this.todolist.length
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
        print("==========onInit==========")
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

    onEnterclick: function() {
        this.hideMsgBox();
        this.msgbox_style1['display'] = 'show';
    },

    onCancleclick: function() {
        this.hideMsgBox();
    },
    onCancleclick1: function() {
        this.msgbox_style1['display'] = 'none';
    },

    showMsgBox: function(obj) {
        this.messageTitle=obj.attributes.title
        this.msgbox_style['display'] = 'show'
        // for(var i=0;i<this.todolist.length ){}
    },

    hideMsgBox: function() {
        this.msgbox_style['display'] = 'none';
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
    }
}