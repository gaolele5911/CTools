@echo off
set ICV_ROOT %1%
::vc_redist.x64.exe /install /quiet /norestart
echo "start ELMonitor....."
start %~dp0%\Bin\ELMonitor.exe
echo "start ELMonitor..."
