@ echo off
:Input
set /p Wow_cn_client="����dbc,maps,mmaps,vmaps�����ڵ�Ŀ¼��"
if not exist %Wow_cn_client% goto Input

mklink /J dbc %Wow_cn_client%\dbc
mklink /J maps %Wow_cn_client%\maps
mklink /J mmaps %Wow_cn_client%\mmaps
mklink /J vmaps %Wow_cn_client%\vmaps
pause
