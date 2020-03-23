Embedded  Javascript 引擎
=========================

1 简介
--------------

`Embedded  Javascript` 引擎（简称ejs）是一个小型并且可嵌入的Javascript引擎，专为物联网设计，赋能物联网企业和开发者全新的开发模式，降低物联网开发门槛，极速提升开发效率。

### 1.1 主要特性

*   `跨硬件`：纯Ｃ开发，零依赖，支持主流 **ROM > 50KB**, **RAM > 2KB**的MCU
* ` 跨平台`：支持裸机运行或无缝集成各种OS
*  `语言规范`：全面兼容ES5/ES6语法特性
*  `高性能`：极速运行，自带垃圾回收（GC）, 全面覆盖基准性能测试
*  `高效率`：极速开发，一个evm.h头文件，无需编译
*  `可扩展`：标准化C接口导入，无缝集成C/C++生态

### 1.2 在线体验

点击 [ejs语法测试](http://47.105.117.50:9999/index#/ejs/home/user) 具体内容


2 用法
-------

### 2.1 目录说明

| 文件      |    说明|
| :-------- | --------:|
| **ejs/bin/**| ejs二进制程序 |
| **lib/include/evm.h**| 开发头文件 |
| **lib/x86_linux/libejs.a**|  Linux平台libejs静态库 （gcc7 64位）|
| **lib/x86_win64/libejs.a**|  windows平台libejs静态库(MinGW-gcc 64位)|
| **ejs/test**|   语法测试代码  | 
| **README.md**|   说明文档 |

### 2.2 快速体验

`ejs` 是命令行解析器. 您可以将Javascript文件参数传递以执行它们：

```
cd bin
./ejs object.js
```


### 2.3 ejs配置文件

| 配置项      |    默认值| 备注|
| :-------- | --------:| :--: |
| heap_size| 1000 * 1000 * 1024 |  堆大小   |
| stack_size|   10000 * 1024 |  栈大小  |
| module_size|    10 | 模块个数  |
| module_paths|     | 模块路径列表  |



```
{
	"heap_size": 1024000000,
	"stack_size": 10240000,
	"module_size": 10,
	"module_paths": [
		".",
		"./ejs_modules"
	]
}
```

3 编译构建
-------


### 3.1  Qt Creater编译说明

+ libejs是基于 gcc 7.3.0  构建的
+ 建议编译环境使用 **Qt 最新版**  http://download.qt.io/archive/qt/5.14/5.14.1/  Qt Creater打开 `ejs.pro` 直接进行编译即可
