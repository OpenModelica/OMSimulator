@ECHO off
REM run this on wsl using:
REM cmd.exe /C buildWinVS.bat VS15-Win64

SET OMS_VS_TARGET=%~1
IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] SET OMS_VS_PLATFORM=32 && SET OMS_VS_VERSION="Visual Studio 14 2015"
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] SET OMS_VS_PLATFORM=64 && SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] SET OMS_VS_PLATFORM=32 && SET OMS_VS_VERSION="Visual Studio 15 2017"
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] SET OMS_VS_PLATFORM=64 && SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

IF NOT DEFINED OMS_VS_VERSION (
  ECHO No argument or unsupported argument given. Use one of the following VS version strings:
  ECHO   "VS14-Win32" for Visual Studio 14 2015
  ECHO   "VS14-Win64" for Visual Studio 14 2015 Win64
  ECHO   "VS15-Win32" for Visual Studio 15 2017
  ECHO   "VS15-Win64" for Visual Studio 15 2017 Win64
  GOTO fail
)

SET OMS_VS_TARGET
SET OMS_VS_VERSION
SET OMS_VS_PLATFORM
ECHO.

SET VSCMD_START_DIR="%CD%"

IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] @CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] @CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

MKDIR install\win\bin

:: -- build OMTLMSimulator ----------------------------------------------------
ECHO # Building OMTLMSimulator
CD OMTLMSimulator
START /B /WAIT CMD /C "buildWinVS.bat %OMS_VS_TARGET%"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD..
COPY /Y OMTLMSimulator\bin\omtlmsimulator.lib install\win\bin
COPY /Y OMTLMSimulator\bin\omtlmsimulator.dll install\win\bin
ECHO %CD%
:: -- build OMTLMSimulator ----------------------------------------------------

:: -- build OMSimulator -------------------------------------------------------
ECHO # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
:: -- build OMSimulator -------------------------------------------------------

install\win\bin\OMSimulator.exe --version
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO omsfail
EXIT /B 0

:fail
ECHO Build failed!
EXIT /B 1

:omsfail
ECHO OMSimulator.exe does not start!
EXIT /B 1
