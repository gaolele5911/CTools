@echo off
echo 开始编译： %1%
echo 请耐心等待...

set logdir=.\Log
set sln=%1%
set name=%2%

"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv" %sln% /rebuild "Release|x86" /out %logdir%\%2%.txt

echo 编译结束 %2%