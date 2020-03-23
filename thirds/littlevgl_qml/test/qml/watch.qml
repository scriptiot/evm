Rectangle {
    id: rect
    x: 0
    y: 0
    width: 800
    height: 600

    color:'lightblue'
    gradient:'lightblue'

    Rectangle{
        x: 200
        y: 100
        width: 400
        height: 400

        color:'lightblue'
        gradient:'lightblue'

        Image{
            source: 'images/wear/ico_watch_background_011.bin'
        }
        
        CircularGauge{
            id: gvsec
            x:45
            y:45
            width:300
            height:300

            CircularGaugeStyle{
                angleRange:360
                labelCount:0
                tickmarkCount:0
                needleColor: 'red'
            }
            minimumValue:0
            maximumValue:360
            value:0

            function animation(){
                var date = new Date();
                var offset = 31;
                var hx = date.getHours();
                var mx = date.getMinutes();
                var sx = date.getSeconds();

                if( hx > 12) hx = hx - 12;

                if( hx <= 6 ) hx = hx * 360 / 12 + 180;
                if( hx > 6 && hx <= 12) hx = hx * 360 / 12 - 180;

                if( sx >= 30 ) sx = sx * 6 - 180;
                else sx = sx * 6 + 180;
                
                if( mx >= 30 ) mx = mx * 6 - 180;
                else mx = mx * 6 + 180;

                hx = hx + date.getMinutes() * 30 / 60;
                

                gvhour.value = hx;
                gvmin.value = mx;
                gvsec.value = sx;
            }

            timer_id:0

            onCompleted: {
                gvsec.timer_id = setInterval(gvsec.animation, 200);
            }
        }


        CircularGauge{
            id:gvmin
            x:95
            y:95
            width:200
            height:200

            CircularGaugeStyle{
                angleRange:360
                labelCount:0
                tickmarkCount:0
                needleColor: 'yellow'
            }
            minimumValue:0
            maximumValue:360
            value:0
        }

        CircularGauge{
            id:gvhour
            x:120
            y:120
            width:150
            height:150

            CircularGaugeStyle{
                angleRange:360
                labelCount:0
                tickmarkCount:0
                needleColor: 'white'
            }
            minimumValue:0
            maximumValue:360
            value:0
        }
    }
}