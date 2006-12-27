@echo off
del c:\temp\*.o
del *.layout
del *.depend
strip *.exe
upx -9 *.exe
pause