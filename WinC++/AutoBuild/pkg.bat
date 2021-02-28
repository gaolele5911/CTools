@echo off

::set t=请输入安装包名称（不带扩展名,命名规则详见：命名规则.txt）:
::begin
::set /p bt=%t%
::set /p sure=确认名称请输入y,按任意键重新输入:
::if %sure%==y goto xm
::set t=请输入安装包名称（不带扩展名,命名规则详见：命名规则.txt）:
::goto begin
::xm
set dir=%~dp0
echo 路径：%dir%
set input=%1%
::传入普通参数
echo 安装包名称:%input%
cd %dir%
cd "build"
python build.py
cd %dir%
python cpy.py
cd %dir%
echo 正在提取PDB符号文件...
python cpy_pdb.py %input%
cd "D:\\Program Files (x86)\\Wise Installation System"
echo XLEasyLinkVideo正在打包...
::Wise32.exe /c /s D:\\Melineye_VNPS打包工具\\客户端\\files\\易联OCX播放器.wse
Wise32.exe /c /s "%dir%\files\XLEasyLinkVideo.wse"
echo 正在复制安装包...
::copy "D:\Melineye_VNPS打包工具\客户端\files\易联OCX播放器.exe" "D:\Melineye_VNPS打包工具\客户端\安装包\%bt%.exe"
copy "%dir%\files\XLEasyLinkVideo.exe" "%dir%\安装包\%input%.exe"
echo %input% XLEasyLinkVideo打包完成!
