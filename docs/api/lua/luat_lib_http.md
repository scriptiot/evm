# http.req

```javascript
http.req(url, params, cb)
```

发起一个http请求,推荐用http.get/post/put/delete方法

## 参数表

Name | Type | Description
-----|------|--------------
`url`|`string`| 目标URL,需要是https://或者http://开头,否则将当成http://开头
`params`|`table`| 可选参数. method方法,headers请求头,body数据,ca证书路径,timeout超时时长,
`cb`|`function`| 回调方法

## 返回值

> `boolean`: 成功启动返回true,否则返回false.启动成功后,cb回调必然会调用一次

## 调用示例

```javascript
//GET请求
http.req("http://www.baidu.com/", null, function(ret, code, headers, body) {
    log.info("http", ret, code, header, body)
}) 
```


--------------------------------------------------
# http.get

```javascript
http.get(url, params, cb)
```

发起一个http get请求

## 参数表

Name | Type | Description
-----|------|--------------
`url`|`string`| 目标URL,需要是https://或者http://开头,否则将当成http://开头
`params`|`table`| 可选参数. headers请求头,body数据,ca证书路径,timeout超时时长,
`cb`|`function`| 回调方法

## 返回值

> `boolean`: 成功启动返回true,否则返回false.启动成功后,cb回调必然会调用一次

## 调用示例

```javascript
//GET请求
http.get("http://www.baidu.com/", nil, function(ret, code, headers, body) {
    log.info("http", ret, code, header, body)
}) 
```


--------------------------------------------------
# http.post

```javascript
http.post(url, params, cb)
```

发起一个http post请求

## 参数表

Name | Type | Description
-----|------|--------------
`url`|`string`| 目标URL,需要是https://或者http://开头,否则将当成http://开头
`params`|`table`| 可选参数. headers请求头,body数据,ca证书路径,timeout超时时长,
`cb`|`function`| 回调方法

## 返回值

> `boolean`: 成功启动返回true,否则返回false.启动成功后,cb回调必然会调用一次

## 调用示例

```javascript
//POST请求
http.post("http://www.baidu.com/", {body: JSON.parse(data)}, function(ret, code, headers, body) {
    log.info("http", ret, code, header, body)
}) 
```


--------------------------------------------------
# http.put

```javascript
http.put(url, params, cb)
```

发起一个http put请求

## 参数表

Name | Type | Description
-----|------|--------------
`url`|`string`| 目标URL,需要是https://或者http://开头,否则将当成http://开头
`params`|`table`| 可选参数. headers请求头,body数据,ca证书路径,timeout超时时长,
`cb`|`function`| 回调方法

## 返回值

> `boolean`: 成功启动返回true,否则返回false.启动成功后,cb回调必然会调用一次

## 调用示例

```javascript
//PUT请求
http.put("http://www.baidu.com/", {body: JSON.parse(data)}, function(ret, code, headers, body) {
    log.info("http", ret, code, header, body)
}) 
```


--------------------------------------------------
# http.delete

```javascript
http.delete(url, params, cb)
```

发起一个http delete请求

## 参数表

Name | Type | Description
-----|------|--------------
`url`|`string`| 目标URL,需要是https://或者http://开头,否则将当成http://开头
`params`|`table`| 可选参数. headers请求头,body数据,ca证书路径,timeout超时时长,
`cb`|`function`| 回调方法

## 返回值

> `boolean`: 成功启动返回true,否则返回false.启动成功后,cb回调必然会调用一次

## 调用示例

```javascript
//DELETE请求
http.put("http://www.baidu.com/", null, function(ret, code, headers, body) {
    log.info("http", ret, code, header, body)
}) 
```


