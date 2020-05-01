### 1. 下载evm-tools

+ 解压到目录

+ 下载Python3 ，如果python3已经安装，跳过。


### 2. 配置git启动配置


+ 启动Git终端

```
cd tools
./evm.sh
```

> 安装完成后，重启Git终端，cd到evm目录


### 3. 编译小熊派

+ 根据指定的board芯片类型进行编译

```
west build -b bearpi_stm32l431 bsp/stm32/bearpi_stm32l431
```

+ 烧写程序

```
west flash
```
