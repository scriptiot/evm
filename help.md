### 1. 安装依赖工具链

+ 同步zephyr子仓库

```

git submodule init
git submodule update
```

+ 安装west

```
pip3 install west -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
```

+ 修改配置文件components/zephyr/.west.yml中第23行

```

url-base: https://gitee.com/evm-zephyr-rtos

```


+ 下载west依赖库

```

west update

```

+ 初始化west

```
cd components/zephyr 
west init -l .
```

+ 测试编译

```

west build -b qemu_x86 samples/hello_world

```


+ 安装qemu

下载 【qemu 64位】添加到系统环境变量

+ 安装 gcc-arm-none-eabi

下载 【gcc-arm-none-eabi】添加到系统环境变量


### 2. 配置编译配置

+ 第一步：windows下配置 ~/.zephyrrc


```
export EVM_BASE=$PWD/../../
export ZEPHYR_BASE=$EVM_BASE/components/zephyr
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=/c/xunlei/gcc-arm-none-eabi-9-2019-q4-major-win32/
export QEMU=/c/Program\ Files/qemu/qemu-system-arm.exe

```

> 保证evm和evm_publish_final在同一层级目录

+ 第二步： 执行zephyr-env.sh

```
cd components/zephyr
source zephyr-env.sh
```

查看.zephyrrc中的环境变量是否生效

```

env|grep ZEPHYR_BASE
env|grep GNUARMEMB_TOOLCHAIN_PATH

```

### 3. 编译程序

+ 根据指定的board芯片类型进行编译

```
rm build -rf
west build -b qemu_cortex_m3 ejs

```

+ 使用qemu模拟运行

```
west build -t run
```
