---
module: wlan
summary: wifi操作库
version: 1.0
date: 2020.03.30
---

--------------------------------------------------
# wlan.getMode

```javascript
wlan.getMode(dev)
```

获取wifi模式

## 参数表

Name | Type | Description
-----|------|--------------
`dev`|`string`| 设备名称,字符串或数值, 可选值0/1, "wlan0","wlan1". 默认"wlan0"

## 返回值

> `int`: 模式wlan.NONE, wlan.STATION, wlan.AP

## 调用示例

```javascript
//获取wlan0的当前模式
var m = wlan.getMode("wlan0")
```


--------------------------------------------------
# wlan.setMode

```javascript
wlan.setMode(dev, mode)
```

设置wifi模式,通常不需要设置

## 参数表

Name | Type | Description
-----|------|--------------
`dev`|`string`| 设备名称,字符串或数值, 可选值0/1, "wlan0","wlan1". 默认"wlan0"
`mode`|`int`| 模式wlan.NONE, wlan.STATION, wlan.AP

## 返回值

> `int`: 设置成功与否,通常不检查

## 调用示例

```javascript
//将wlan设置为wifi客户端模式
wlan.setMode("wlan0",wlan.STATION) 
```


--------------------------------------------------
# wlan.connect

```javascript
wlan.connect(ssid, password)
```

连接wifi,成功启动联网线程不等于联网成功!!

## 参数表

Name | Type | Description
-----|------|--------------
`ssid`|`string`| ssid  wifi的SSID
`password`|`string`| password wifi的密码,可选

## 返回值

> `boolean`: 如果正常启动联网线程,无返回值,否则返回出错信息.

## 调用示例

```javascript
//连接到uiot,密码1234567890
wlan.connect("uiot", "1234567890")
```


--------------------------------------------------
# wlan.disconnect

```javascript
wlan.disconnect()
```

断开wifi

## 参数表

> 无参数

## 返回值

> `boolean`: 成功返回true,否则返回false

## 调用示例

```javascript
//断开wifi连接
wlan.disconnect()
```


--------------------------------------------------
# wlan.connected

```javascript
wlan.connected()
```

是否已经连上wifi网络

## 参数表

> 无参数

## 返回值

> `boolean`: 已连接返回true0,未连接返回false

## 调用示例

```javascript
//连上wifi网络,只代表密码正确, 不一定拿到了ip
wlan.connected()
```


--------------------------------------------------
# wlan.autoreconnect

```javascript
wlan.autoreconnect(enable)
```

设置或查询wifi station是否自动连接

## 参数表

Name | Type | Description
-----|------|--------------
`enable`|`int`| 传入1启用自动连接(自动重连wifi), 传入0关闭. 不传这个参数就是查询

## 返回值

> `int`: 已启用自动连接(自动重连wifi)返回1, 否则返回0

## 调用示例

```javascript
//查询自动连接的设置
wlan.autoreconnect()
-------------------------
//设置自动连接
wlan.autoreconnect(1)
```


--------------------------------------------------
# wlan.scan

```javascript
wlan.scan()
```

开始扫网,通常配合wlan.scanResult使用

## 参数表

> 无参数

## 返回值

> `boolean`: 启动结果,一般为true

## 调用示例

```javascript
// 扫描并查询结果
timer = require('timer')
wlan.scan()
timer.mdelay(30000)
re = wlan.scanResult()
console.log(re);
```


--------------------------------------------------
# wlan.scanResult

```javascript
wlan.scanResult(num)
```

获取扫网结果,需要先执行wlan.scan,并等待WLAN_SCAN_DONE事件

## 参数表

Name | Type | Description
-----|------|--------------
`num`|`int`| 最大结果数量,默认50

## 返回值

> `table`: 扫描结果的数组

## 调用示例

```javascript
//扫描并查询结果
timer = require('timer')
wlan.scan()
timer.mdelay(30000)
re = wlan.scanResult()
console.log(re);
```


--------------------------------------------------
# wlan.get_mac

```javascript
wlan.get_mac()
```

获取mac地址

## 参数表

> 无参数

## 返回值

> `string`: 长度为12的HEX字符串,如果不存在就返回值nil

## 调用示例

```javascript
//获取MAC地址
log.info("wlan", "mac addr", wlan.get_mac())
```


--------------------------------------------------
# wlan.get_mac_raw

```javascript
wlan.get_mac_raw()
```

获取mac地址,raw格式

## 参数表

> 无参数

## 返回值

> `string`: 6字节的mac地址串

## 调用示例

```javascript
//查询mac地址, 二进制模式
var mac_raw  = wlan.get_mac_raw()
if( mac_raw ) {
    log.info("wlan", "mac addr", mac_raw:toHex())
}
```


--------------------------------------------------
# wlan.ready

```javascript
wlan.ready()
```

wifi是否已经获取ip

## 参数表

> 无参数

## 返回值

> `boolean`: 已经有ip返回true,否则返回false

## 调用示例

```javascript
//查询是否已经wifi联网
if( wlan.ready() ) {
    log.info("wlan", "wifi ok", "Let's Rock!")
}
```


--------------------------------------------------
# wlan.oneShotStart

```javascript
wlan.oneShotStart(mode, ssid, passwd)
```

启动配网过程,支持UDP/SOCKET/APWEB配网

## 参数表

Name | Type | Description
-----|------|--------------
`mode`|`int`| 配网模式: 0-UDP配网, 1-SOCKET配网, 2-AP网页配网
`ssid`|`string`| AP网页配网时的SSID,默认值为luatos
`passwd`|`string`| AP网页配网时的密钥,默认值为12345678

## 返回值

> `boolean`: 启动成功返回true,否则返回false

## 调用示例

```javascript
//UDP配网,需要下载联德盛测试APP,2.0版本
wlan.oneShotStart(0)
-------------------------
//SOCKET配网,需要下载联德盛测试APP,2.0版本
wlan.oneShotStart(1)
-------------------------
//AP网页配网,手机搜索wifi "W600APWEB", 密码12345678. 连上之后,保持wifi连接,浏览器访问 192.168.168.1, 按提示输入.
wlan.oneShotStart(2, "W600APWEB", "12345678")
-------------------------
```


--------------------------------------------------
# wlan.oneshotStop

```javascript
wlan.oneshotStop()
```

停止配网, 通常不需要调用

## 参数表

> 无参数

## 返回值

> *无返回值*

## 调用示例

```javascript
//停止配网
wlan.oneshotStop()
```


--------------------------------------------------
# wlan.oneshotState

```javascript
wlan.oneshotState()
```

查询配网状态

## 参数表

> 无参数

## 返回值

> `boolean`: 配网中返回true,否则返回false

## 调用示例

```javascript
//查询
if(wlan.oneshotState()) {
    log.info("wlan", "配网中")
}
```


--------------------------------------------------
# wlan.rssi

```javascript
wlan.rssi()
```

获取wifi信号强度值rssi

## 参数表

> 无参数

## 返回值

> `int`: 如果是station模式,返回正的rssi值,否则返回负值

## 调用示例

```javascript
//信号强度
log.info("wlan", wlan.rssi())
```