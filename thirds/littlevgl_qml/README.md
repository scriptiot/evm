Littlevgl QML
=========================
###  1. 什么是QML

QML是一种描述性的脚本语言，文件格式以.qml结尾。
语法格式融合了CSS、JavaScript。通过QML语言可以快速构建UI界面，开发者在不需要掌握大量设计技巧的前提下也能很容易的掌握QML技术。


### 2. 来源

QML是Qt推出的Qt Quick技术的一部分，是一种新增的简便易学的语言。
QML是一种陈述性语言，用来描述一个程序的用户界面：无论是什么样子，以及它如何表现。
在QML，一个用户界面被指定为具有属性的对象树。 这使得Qt更加便于很少或没有编程经验的人使用。


### 3. EVM的QML语言

QML作为QT主打的面向UI开发的语言，其强大的描述能力和调用JavaScript的功能，为UI开发带来了全新的设计理念。尤其是QML属性绑定式编程，可以快速的描述UI的行为和动效。然而QML只能运行在QT图形引擎中，其体积庞大、性能不足的问题一直为人诟病，更不用说能让QML在单片机上运行了。
但EVM引擎打破了这种限制，EVM引擎可以完整运行QML语法，同时通过虚拟机扩展接口，可以让QML语言服务于各种底层图形引擎，例如GuiLite、Littlevgl、AWTK等等。同时EVM引擎能够让多种语言混合运行，所以完美解决了QML中调用JavaScript的功能，并且还打破了QT中的QML无法调用JavaScript库的问题。


### 4.  目录说明

| 文件      |    说明|
| :-------- | --------:|
| **ejs**| ejs目录 |
| **ejs/bin/ejs**| ejs二进制程序 |
| **ejs/bin/ejs.json**| ejs二进制程序堆栈信息配置 |
| **ejs/bin/ejs_modules**| ejs二进制程序库寻找路径 |
| **ejs/test**|   ejs语法测试用例  | 
| **ejs/README.md**|   ejs说明文档 |
| **inlcude**|    evm虚拟机接口头文件目录|
| **inlcude/evm.h**|    evm虚拟机接口头文件|
| **include/qml_module.h**| qml接口头文件|
| **lib**|    静态库目录 |
| **lib/arm_windows_armcc/libqml.lib**|  windows平台libqml静态库(armcc, Keil)|
| **lib/x86_64_linux_gnu/linux/libejs.a**|  Linux平台libejs静态库 （gcc7 64位）|
| **lib/x86_64_linux_gnu/linux/libeqml.a**|  Linux平台libqml静态库 （gcc7 64位）|
| **lib/x86_64_windows_mingw/libejs.a**|  windows平台libejs静态库(MinGW-gcc 64位)|
| **lib/x86_64_windows_mingw/libeqml.a**|  windows平台libeqml静态库(MinGW-gcc 64位)|
| **thirds/littevgl_qml**| littevgl_qml模块| 
| **thirds/littevgl_qml/lvgl_module**| 基于littlelvgl的qml封装| 
| **thirds/littevgl_qml/ports**| 第三方平台适配| 
| **thirds/littevgl_qml/ports/Qt**| Qt平台适配|
| **thirds/littevgl_qml/ports/stm32**| stm32平台(Keil工程)适配| 
| **thirds/littevgl_qml/ports/stm32/stm32f103-fire-arbitrary**| stm32平台(Keil工程)野火开发板适配| 
| **thirds/littevgl_qml/test/**|    测试用例目录 |
| **thirds/littevgl_qml/README.md**|    littevgl_qml说明文档 |
| **README.md**|   evm说明文档 |

#### 5.  编译构建

##### 5.1  Qt运行 littevgl_qml

+ libqml是基于 gcc 7.3.0  运行
+ 建议编译环境使用 **Qt 最新版**  http://download.qt.io/archive/qt/5.14/5.14.1/  Qt Creater打开 `evm/thirds/littlevgl_qml/ports/Qt/littlevgl_qml.pro` 直接进行编译即可

+ 自定义qml模块加载路径

	用户如果需要自定义qml路径，可以修改 `evm/thirds/littlevgl_qml/ports/Qt/evm_main.c`中的qml模块路径定义，需要保持 `modules_paths_count`和 `modules_paths`长度一致
```
int modules_paths_count = 3;
char* modules_paths[] = {
    ".",
    "./test",
    "../../../../../../test/qml",
};
```

+  程序默认启动实例 qml 文件
```
evm/test/qml/login.qml
```

### 6. Littlevgl的QML

支持的对象：

    *  Item
        *  x，x坐标，整数
        *  y，y坐标，整数
        *  width，宽度，整数
        *  height，高度，整数
        *  opacity，透明度，小数0~1
        *  visible，可见性，布尔值
        
    *  Rectangle，矩形，继承Item
        *  color，背景颜色，整数或者字符串
        *  gradient，背景渐变色，整数或者字符串
        *  radius，弧度，整数
        *  border.color，边框颜色，整数或者字符串
        *  border.width，边框宽度，整数
        
    *  Button，按钮，继承Item
        *  onPressed，鼠标按下事件
        *  onReleased，鼠标释放事件
        *  onClicked，鼠标点击事件
        
    *  Text，文本，继承Item
        *  color，文字颜色，整数或者字符串
        *  text，内容，字符串
        
    *  Image，图片，继承Item
        *  source，图片源，字符串
        
    *  CheckBox，勾选框，继承Item
        *  text，内容，字符串
        *  checked，勾选，布尔值
            
    *  ScrollView，滚动视图，继承Item
    *  TextField，单行输入框，继承Item
        *  text，内容，字符串
        *  placeholderText，驻留文本，字符串
        *  onTextEdited，文本编辑事件
        
    *  CircularGauge，弧形仪表，继承Item
        *  value，仪表值，整数
        *  maximumValue，最大值，整数
        *  minimumValue，最小值，整数
        
    *  CircularGaugeStyle，仪表样式
        *  angleRange，角度范围，整数
        *  labelCount，标签数目，整数
        *  tickmarkCount，指针标记数目，整数
        *  needleColor，指针颜色，整数或者字符串
        
    *  Window，窗口，继承Item
        *  title，窗口标题，字符串
        
###  7. 版权声明

Littlevgl_qml 采用 MIT Licence 协议
