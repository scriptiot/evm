router = require("@system.router")

export default {
    data: {
        count: 3,
        title: 'World',
        visible_count: 3,
        options1: ['2021', '2022', '2023', '2024', '2025', '2026', '2027'],
        selected1: 1,
        options2: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12'],
        selected2: 2,
        options3: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30'],
        selected3: 3,
        // onclick: undefined,
        increased: true,
        opacity: 0.1,
        minus_btn_style: { left: '40px' },
        plus_btn_style: { left: '140px' },
        topbar_style: { 'background-color': 'transparent' },
    },
    onclick: function () {
        print("=====onclick=====");
    },
    onlongpress: function () {
        print(this.selected1);
    },
    onNext: function (obj, value) {
        print("=====onValueChanged=====");
        print(value);
    },
    onReturn: function() {
        print("=====onReturn=====");
        router.push({
            path: "evue-dock"
        })
    },
    minus: function () {
        this.count = this.count - 1
    },
    plus: function () {
        this.count = this.count + 1
    },
}