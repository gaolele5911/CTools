@echo off
echo ��ʼ���룺 %1%
echo �����ĵȴ�...

set logdir=.\Log
set sln=%1%
set name=%2%

"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv" %sln% /rebuild "Release|x86" /out %logdir%\%2%.txt

echo ������� %2%