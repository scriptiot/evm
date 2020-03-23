url="http://47.105.117.50:9999/api/v1/evm/build"
build_time=`date +%Y-%m-%dT%H:%M:%S.000%z`
device_info=`uname -a`

curl -o /dev/null -s --include --request POST --header "Content-Type: application/json" --data "{
	\"device_info\" :\"$device_info\",
	\"project_info\" :\"$1\",
	\"time\" :\"$build_time\",
	\"platform\" : \"linux\",
	\"project\" : \"little_qml\"
}" $url
