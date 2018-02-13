@echo off
REM run this on wsl using:
REM cmd.exe /C buildWinVS.bat VS15-Win64

if ["%~1"]==["VS14-Win64"] SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
if ["%~1"]==["VS15-Win64"] SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

REM exit if no VS version could be recognized
IF NOT DEFINED OMS_VS_VERSION echo Unsupported VS version.
IF NOT DEFINED OMS_VS_VERSION exit 1

echo Using %OMS_VS_VERSION%

SET "VSCMD_START_DIR=%CD%"
if ["%~1"]==["VS14-Win64"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
if ["%~1"]==["VS15-Win64"] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

echo # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release
