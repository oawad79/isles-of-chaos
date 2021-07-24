@echo off

xcopy ..\resources\ ..\out\build\x64-Debug\resources\ /E /Y
xcopy ..\resources\ ..\cmake-build-debug\resources\ /E /Y
