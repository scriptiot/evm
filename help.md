### 1. 下载evm-tools

+ 解压到任意目录

+ 下载Python3 ，如果python3已经安装，跳过。


### 2. 一键安装EVM开发环境

+ evm.sh用法

```sh
USAGE: ./evm.sh [path of evm-tools]
e.g.: ./evm.sh /c/evm-tools

```
+ 举例

```
cd tools
./evm.sh /c/evm-tools
```

+ 安装完成后，重启Git终端，cd到evm目录


### 3. 编译小熊派

+ 根据指定的board芯片类型进行编译

```
west build -b bearpi_stm32l431 bsp/stm32/bearpi_stm32l431 -d build/stm32/bearpi_stm32l431
```

+ 烧写程序

```
west flash
```

### 4. 快速体验ejs

+ 根据指定的board芯片类型进行编译

```
west build -b qemu_cortex_m3 bsp/qemu_cortex_m3/ejs -d build/qemu_cortex_m3/ejs
```

+ 运行程序

```
west build -t run -d build/qemu_cortex_m3/ejs
```
