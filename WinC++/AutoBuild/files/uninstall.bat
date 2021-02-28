@ECHO OFF 

reg delete HKEY_CLASSES_ROOT\MtiWatchEasyLink /f
reg delete HKEY_CURRENT_USER\Software\XLEasyLinkVideo /f


@ECHO kill chrom begin............................. 
SET status=1 
(TASKLIST|FIND /I "chrome.exe"||SET status=0) 2>nul 1>nul
ECHO %status%
IF %status% EQU 1 (tasklist|findstr /i chrome.exe && taskkill /f /im chrome.exe) ELSE (ECHO chrome is not run.)
@ECHO kill chrome end...............................

@ECHO kill EasyLinkMonitor begin............................. 
SET status=1 
(TASKLIST|FIND /I "ELMonitor.exe"||SET status=0) 2>nul 1>nul
ECHO %status%
taskkill /f /im ELMONI~*
taskkill /f /im ELMonitor.exe
@ECHO kill ELMonitor.exe end...............................


@ECHO kill EasyLinkVideo begin............................. 
SET status=1 
(TASKLIST|FIND /I "EasyLinkVideo.exe"||SET status=0) 2>nul 1>nul
ECHO %status%
taskkill /f /im EASYLI~*
taskkill /f /im EasyLinkVideo.exe
@ECHO kill EasyLinkVideo end...............................