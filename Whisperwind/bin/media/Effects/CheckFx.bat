@echo off

:tag

fxc.exe /Od /Zi /T fx_2_0 /Fo check.fxo "%~1"
pause

goto tag 
