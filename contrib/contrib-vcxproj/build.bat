@echo off
cd %~dp0
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
msbuild %*