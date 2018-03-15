@ECHO off
REM run this on wsl using:
REM cmd.exe /C buildWinVS.bat VS15-Win64

IF ["%~1"]==["VS14-Win32"] SET OMS_VS_VERSION="Visual Studio 14 2015"
IF ["%~1"]==["VS14-Win64"] SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
IF ["%~1"]==["VS15-Win64"] SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"


IF NOT DEFINED OMS_VS_VERSION (
  ECHO No argument or unsupported argument given. Use one of the following VS version strings:
  ECHO   "VS14-Win32" for Visual Studio 14 2015
  ECHO   "VS14-Win64" for Visual Studio 14 2015 Win64
  ECHO   "VS15-Win64" for Visual Studio 15 2017 Win64
  GOTO fail
)

ECHO Using %OMS_VS_VERSION%

SET "VSCMD_START_DIR=%CD%"
IF ["%~1"]==["VS14-Win32"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF ["%~1"]==["VS14-Win64"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
IF ["%~1"]==["VS15-Win64"] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

ECHO # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

EXIT /b 0

:fail
ECHO Build failed!
EXIT /b 1
