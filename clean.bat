@echo off
FOR /F "tokens=*" %%g IN ('pwd') do (SET TTYSNDDIR=%%g)
PATH=%TTYSNDDIR%\MINGW\bin;%PATH%
@echo on
mingw32-make -f Makefile.win clean
@echo off
pause