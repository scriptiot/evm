echo "==========【install west】==============="
pip3 install west -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
echo "==========【install west successfully】==============="
echo "==========【install zephyr tools】==============="
EVMTOOLSDIR=$PWD
EVMDIR=$PWD/..
cd ${EVMDIR}/components/zephyr/scripts && pip3 install -r requirements.txt -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
cd ${EVMTOOLSDIR}
echo "==========【install west tools successfully】==============="

EVM_BASE=$(cd $PWD/..; pwd)
EVM_TOOLS=/c/evm-tools

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
    echo ${EVMRC} >> ${bash_path}
    echo "==========【install .evmrc successfully】==============="
else
    echo "==========【.evmrc is installed】==============="
fi

echo "==========【git submodule init components/zephyr】==============="
cd ${EVM_BASE}
git submodule init components/zephyr
echo "==========【git submodule init components/zephyr successfully】==============="
echo "==========【git submodule upate】==============="
git submodule update
echo "==========【git submodule upate successfully】==============="

echo "==========【west update】==============="
export ZEPHYR_BASE=$EVM_BASE/components/zephyr
sed -i "s! url-base: https://github.com/zephyrproject-rtos! url-base: https://gitee.com/evm-zephyr-rtos! g" ${ZEPHYR_BASE}/west.yml
west update
echo "==========【west update successfully】==============="

echo -e "\n\nCongratulations! You have install 【EVM】 environment successfully!"

echo -e "\r\n\
 ====== \\\\        //   ===== =====  \r\n\
||       \\\\      //   ||    ||   || \r\n\
||===     \\\\    //    ||    ||   || \r\n\
||         \\\\  //     ||    ||   || \r\n\
 ======     \\\\//      ||    ||   || \r\n\
\r\n"

echo -e "Please restart Git windows!\n"