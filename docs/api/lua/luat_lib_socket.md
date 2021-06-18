---
module: socket
summary: socket操作库
version: 1.0
date: 2020.03.30
---

--------------------------------------------------
# socket.tsend

```javascript
socket.tsend(host, port, data)
```

直接向地址发送一段数据

## 参数表

Name | Type | Description
-----|------|--------------
`host`|`string`| 服务器域名或者ip
`port`|`int`| 服务器端口号
`data`|`string`| 待发送的数据

## 返回值

> *无返回值*

## 调用示例

```javascript
//
socket.tsend("www.baidu.com", 80, "GET / HTTP/1.0\r\n\r\n")
```


--------------------------------------------------
# socket.isReady

```javascript
socket.isReady()
```

网络是否就绪

## 参数表

> 无参数

## 返回值

> `boolean`: 已联网返回true,否则返回false


--------------------------------------------------
# socket.ip

```javascript
socket.ip()
```

获取自身ip,通常是内网ip

## 参数表

> 无参数

## 返回值

> `string`: 已联网返回ip地址,否则返回nil


--------------------------------------------------
# socket.tcp

```javascript
socket.tcp()
```

新建一个tcp socket

## 参数表

> 无参数

## 返回值

> `object`: socket对象,如果创建失败会返回nil

## 调用示例

```javascript
//如果读取失败,会返回nil
var so = socket.tcp()
if(so) {
    so.host(so, "www.baidu.com")
    so.port(so, 80)
    so.on(so, "connect", function(id, re){
        if(re == 1){
            so.send(so, "GET / HTTP/1.0\r\n\r\n")
        }
    })
    so.on(so, "recv", function(id, data){
        log.info("netc", id, data)
    })
    if(so.start(so) == 1) {
        timer.mdelay(10000);
    }
    so.close(so)
    so.clean(so)
}
```


--------------------------------------------------
# socket.udp

```javascript
socket.udp()
```

新建一个udp socket

## 参数表

> 无参数

## 返回值

> *无返回值*


--------------------------------------------------
# so:start

```javascript
so.start(so, host, port)
```

启动socket线程

## 参数表

Name | Type | Description
-----|------|--------------
`host`|`string`| 服务器域名或ip,如果已经使用so:host和so:port配置,就不需要传参数了
`port`|`port`| 服务器端口,如果已经使用so:host和so:port配置,就不需要传参数了

## 返回值

> `int`: 成功返回1,失败返回0

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:close

```javascript
so.close(so)
```

关闭socket对象

## 参数表

> 无参数

## 返回值

> *无返回值*

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:send

```javascript
so.send(so, data, flags)
```

通过socket对象发送数据

## 参数表

Name | Type | Description
-----|------|--------------
`data`|`string`| 待发送数据
`flags`|`int`| 可选的额外参数,底层相关.例如NBIOT下的rai值, 传入2,代表数据已经全部发送完成,可更快进入休眠.

## 返回值

> `boolean`: 发送成功返回true,否则返回false

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:id

```javascript
so.id(so)
```

获取socket对象的id

## 参数表

> 无参数

## 返回值

> `string`: 对象id,全局唯一

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:host

```javascript
so.host(so, host)
```

设置服务器域名或ip

## 参数表

Name | Type | Description
-----|------|--------------
`host`|`string`| 服务器域名或ip

## 返回值

> *无返回值*

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:port

```javascript
so.port(so, port)
```

设置服务器端口

## 参数表

Name | Type | Description
-----|------|--------------
`port`|`int`| 服务器端口

## 返回值

> *无返回值*

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:clean

```javascript
so.clean(so, 0)
```

清理socket关联的资源,socket对象在废弃前必须调用

## 参数表

Name | Type | Description
-----|------|--------------
`0`|`null`| *无*

## 返回值

> *无返回值*

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:on

```javascript
so.on(so, event, func)
```

设置socket的事件回调

## 参数表

Name | Type | Description
-----|------|--------------
`event`|`string`| 事件名称
`func`|`function`| 回调方法

## 返回值

> *无返回值*

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:closed

```javascript
so.closed(so)
```

socket是否已经断开?

## 参数表

> 无参数

## 返回值

No. | Type | Description
----|------|--------------
1 |`int`| 未断开0,已断开1
2 |`bool`| 未断开返回false,已断开返回true, V0003新增

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:rebind

```javascript
so.rebind(so, socket_id)
```

为netclient绑定socket id, 该操作仅在NBIOT模块下有意义.

## 参数表

Name | Type | Description
-----|------|--------------
`socket_id`|`int`| socket的id.

## 返回值

> `bool`: 成功返回true, 否则返回false. V0003新增

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```


--------------------------------------------------
# so:sockid

```javascript
so.sockid(so)
```

获取底层socket id

## 参数表

> 无参数

## 返回值

> `int`: 底层socket id

## 调用示例

```javascript
//参考socket.tcp的说明, 并查阅demo

```