var timerList = []

function wlSetTimeout(cb, period) {
    timerList.push({
        cb: cb,
        period: period,
        once: true,
        count: period
    })
}

function wlSetInterval(cb, period) {
    timerList.push({
        cb: cb,
        period: period,
        once: false,
        count: period
    })
}

// ------------------------
function wlCallback() {
    var index = 0;
    while (timerList.length) {
        if (timerList[index].count != 0) {
            timerList[index].count--
        } else {
            if (timerList[index].once) {
                timerList[index].cb()
                timerList.splice(index, 1)
            } else {
                timerList[index].count = timerList[index].period
            }
        }
        index++;
    }    
}

setInterval(wlCallback, 1)