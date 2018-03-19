@ECHO off
REM run this on wsl using:
REM cmd.exe /C buildWinVS.bat VS15-Win64

SET OMS_VS_TARGET=%~1
IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] SET OMS_VS_PLATFORM=32 && SET OMS_VS_VERSION="Visual Studio 14 2015"
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] SET OMS_VS_PLATFORM=64 && SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] SET OMS_VS_PLATFORM=64 && SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

IF NOT DEFINED OMS_VS_VERSION (
  ECHO No argument or unsupported argument given. Use one of the following VS version strings:
  ECHO   "VS14-Win32" for Visual Studio 14 2015
  ECHO   "VS14-Win64" for Visual Studio 14 2015 Win64
  ECHO   "VS15-Win64" for Visual Studio 15 2017 Win64
  GOTO fail
)

SET OMS_VS_TARGET
SET OMS_VS_VERSION
SET OMS_VS_PLATFORM
ECHO.

SET VSCMD_START_DIR="%CD%"

IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

:: -- build OMSimulator -------------------------------------------------------
ECHO # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
:: -- build OMSimulator -------------------------------------------------------

EXIT /b 0

:fail
ECHO Build failed!
EXIT /b 1
