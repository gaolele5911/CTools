@echo off

::set t=�����밲װ�����ƣ�������չ��,���������������������.txt��:
::begin
::set /p bt=%t%
::set /p sure=ȷ������������y,���������������:
::if %sure%==y goto xm
::set t=�����밲װ�����ƣ�������չ��,���������������������.txt��:
::goto begin
::xm
set dir=%~dp0
echo ·����%dir%
set input=%1%
::������ͨ����
echo ��װ������:%input%
cd %dir%
cd "build"
python build.py
cd %dir%
python cpy.py
cd %dir%
echo ������ȡPDB�����ļ�...
python cpy_pdb.py %input%
cd "D:\\Program Files (x86)\\Wise Installation System"
echo XLEasyLinkVideo���ڴ��...
::Wise32.exe /c /s D:\\Melineye_VNPS�������\\�ͻ���\\files\\����OCX������.wse
Wise32.exe /c /s "%dir%\files\XLEasyLinkVideo.wse"
echo ���ڸ��ư�װ��...
::copy "D:\Melineye_VNPS�������\�ͻ���\files\����OCX������.exe" "D:\Melineye_VNPS�������\�ͻ���\��װ��\%bt%.exe"
copy "%dir%\files\XLEasyLinkVideo.exe" "%dir%\��װ��\%input%.exe"
echo %input% XLEasyLinkVideo������!
