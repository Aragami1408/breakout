@echo off
pushd ..\build
cl -FC -Zi ..\code\win32_platform.c user32.lib gdi32.lib
del *.obj
popd 
