### 1. 下载evm-tools

+ 解压到C盘


### 2. 配置git启动配置


+ 启动Git终端

```
touch ~/.bash_profile
touch ~/.bashrc

```

+ 编辑~/.bashrc


```
export EVM_BASE=~/Desktop/workspace/evm_publish_final
export EVM_TOOLS=/c/evm-tools
export ZEPHYR_BASE=$EVM_BASE/components/zephyr
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=$EVM_TOOLS/gcc-arm-none-eabi-9-2019-q4-major-win32/
export PATH=$EVM_TOOLS/ninja:$PATH
export PATH=$EVM_TOOLS/CMake/bin:$PATH
export PATH=$EVM_TOOLS/gcc-arm-none-eabi-9-2019-q4-major-win32/bin:$PATH
export PATH=$EVM_TOOLS/qemu:$PATH
export PATH=$EVM_TOOLS/Python38:$EVM_TOOLS/Python38/Scripts:$PATH

```

| 文件      |    说明|
| :-------- | --------:|
| **EVM_BASE**| evm项目目录 |
| **EVM_TOOLS**| evm-tools工具链安装目录 |

+ 重启Git终端即可


### 3. 编译运行程序

+ 根据指定的board芯片类型进行编译

```
rm build -rf
west build -b qemu_cortex_m3 ejs

```

+ 使用qemu模拟运行

```
west build -t run
```
