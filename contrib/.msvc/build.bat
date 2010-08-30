cd %~dp0
rd /S /Q ..\lib
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
msbuild %*