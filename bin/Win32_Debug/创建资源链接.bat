@ echo off
:Input
set /p Wow_cn_client="输入dbc,maps,mmaps,vmaps等所在的目录："
if not exist %Wow_cn_client% goto Input

mklink /J dbc %Wow_cn_client%\dbc
mklink /J maps %Wow_cn_client%\maps
mklink /J mmaps %Wow_cn_client%\mmaps
mklink /J vmaps %Wow_cn_client%\vmaps
pause
