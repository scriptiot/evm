Rectangle {
    id: rect
    x: 0
    y: 0
    width: 240
    height: 320

    color:'lightblue'
    gradient:'lightblue'

    Rectangle{
        x: 60
        y: 100
        width: 160
        height: 160

        color:'lightblue'
        gradient:'lightblue'

        Image{
            source: 'watch.bin'
        }
        
        CircularGauge{
            id: gvsec
            x:10
            y:10
            width:100
            height:100

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
            x:20
            y:20
            width:80
            height:80

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
            x:30
            y:30
            width:60
            height:60

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