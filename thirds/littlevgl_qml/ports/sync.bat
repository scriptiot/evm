@echo off
set url="http://47.105.117.50:9999/api/v1/evm/build"

for /f "tokens=2-4 delims=/ "  %%a in ("%date%") do (set MM=%%a& set DD=%%b& set YYYY=%%c)
set YY=%YYYY:~0,2%
if not "%YY%" == "20" (rem For Chinese date format
    for /f "tokens=1-3 delims=/ "  %%a in ("%date%") do (set YYYY=%%a& set MM=%%b& set DD=%%c))

set datetime=%YYYY%-%MM%-%DD%T%time: =0%0+0800
set devie_info=Win-%USERNAME%
set raw_data={^
\"device_info\" :\"%devie_info%\",^
\"project_info\" :\"%1\",^
\"time\" :\"%datetime%\",^
\"platform\" : \"windows\",^
\"project\" : \"little_qml\"^
}

curl.exe -o /dev/null -s --include --request POST --header "Content-Type: application/json" --data-binary^
 "%raw_data%" "%url%"

exit /B 0