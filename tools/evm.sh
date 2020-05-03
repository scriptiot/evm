
if [ $# != 1 ] ; then 
    echo "USAGE: $0 [path of evm-tools]" 
    echo " e.g.: $0 /c/evm-tools"
    exit 1; 
else
    if [ -d $1 ];then
        EVM_TOOLS=$1
    else
        echo "evm-tools path:[$1] is not existed! Please enter correct path!"
        exit 1;
    fi
fi

echo "==========【install west】==============="
pip3 install west -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
echo "==========【install west successfully】==============="

EVMTOOLSDIR=$PWD
EVMDIR=$PWD/..
cd ${EVMTOOLSDIR}
EVM_BASE=$(cd $PWD/..; pwd)
EVM_TOOLS=$1

echo "==========【git submodule init components/zephyr-rtos/zephyr】==============="
cd ${EVM_BASE}

ZEPHYR=components/zephyr-rtos/zephyr
git submodule init ${ZEPHYR}
echo "==========【git submodule init ${ZEPHYR} successfully】==============="
echo "==========【git submodule upate】==============="
git submodule update
echo "==========【git submodule upate successfully】==============="

echo "==========【install zephyr tools】==============="
cd ${EVMDIR}/components/zephyr-rtos/zephyr/scripts && pip3 install -r requirements.txt -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
echo "==========【install west tools successfully】==============="


bash_path="${HOME}/.bash_profile"
evm_path="${HOME}/.evmrc"
if [ -f ${bash_path} ];then
    if [ -f ${evm_path} ];then
        rm ${evm_path}
    fi
else
    echo "="
    touch ${bash_path}
fi

echo "==========【generate .evmrc】==============="
cd ${EVMTOOLSDIR}
rm -rf .evmrc.tmp
touch .evmrc.tmp
cp .evmrc .evmrc.tmp
sed -i "1iexport EVM_BASE=${EVM_BASE}" .evmrc.tmp
sed -i "2iexport EVM_TOOLS=${EVM_TOOLS}" .evmrc.tmp
cp .evmrc.tmp ~/.evmrc
rm .evmrc.tmp
echo "==========【generate .evmrc successfully】==============="

echo "==========【install .evmrc】==============="
EVMRC="test -f ~/.evmrc && . ~/.evmrc"
isEvmInBashrc=$(grep 'test -f ~/.evmrc && . ~/.evmrc' ~/.bash_profile -n)
if [ -z "${isEvmInBashrc}" ]; then 
    echo -e "\n${EVMRC}" >> ${bash_path}
    echo "==========【install .evmrc successfully】==============="
else
    echo "==========【.evmrc is installed】==============="
fi



echo "==========【west update】==============="
cd ${EVM_BASE}
export ZEPHYR_BASE=${ZEPHYR}
west init -l ${ZEPHYR}
sed -i "s! url-base: https://github.com/zephyrproject-rtos! url-base: https://gitee.com/evm-zephyr-rtos! g" ${ZEPHYR_BASE}/west.yml
west update
echo "==========【west update successfully】==============="

echo -e "\n\nCongratulations! You have install 【EVM】 environment successfully!"

echo -e "\r\n\
 _____ __     __ __  __ \r\n\
| ____|\\ \\   / /|  \\/  | \r\n\
|  _|   \\ \\ / / | |\\/| | \r\n\
| |___   \\ V /  | |  | | \r\n\
|_____|   \\_/   |_|  |_| \r\n\
                         \r\n\
Github: https://github.com/scriptiot/evm \r\n\
Copyright (C) 2020 @scriptiot \r\n\
\r\n"

echo -e "Please restart Git windows!\n"
