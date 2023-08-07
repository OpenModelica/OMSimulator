@ECHO off
REM run this on wsl using:
REM cmd.exe /C "configWinVS.bat VS15-Win64"

SET OMS_VS_TARGET=%~1
SET TARGET=%~2
IF NOT DEFINED TARGET SET TARGET=all

echo # Target: %TARGET%

IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] SET OMS_VS_PLATFORM=32&& SET OMS_VS_VERSION="Visual Studio 14 2015"
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] SET OMS_VS_PLATFORM=64&& SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] SET OMS_VS_PLATFORM=32&& SET OMS_VS_VERSION="Visual Studio 15 2017"
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] SET OMS_VS_PLATFORM=64&& SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

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

:: -- init ----------------------------
SET VSCMD_START_DIR="%CD%"

IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] @CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] @CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] (
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
  ) ELSE (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
  )
)
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] (
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
  ) ELSE (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
  )
)


IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

ECHO.
SET CMAKE_BUILD_TYPE
ECHO.
:: -- init ----------------------------

IF NOT EXIST build\\ mkdir build
IF NOT EXIST build\\win\\ mkdir build\\win
IF NOT EXIST install\\ MKDIR install
IF NOT EXIST install\\win MKDIR install\\win
IF NOT EXIST install\\win\\lib MKDIR install\\win\\lib

@REM do not change the order to be consistent with line 265 config all
IF ["%TARGET%"]==["clean"] GOTO clean
IF ["%TARGET%"]==["xerces"] GOTO xerces
IF ["%TARGET%"]==["pthread"] GOTO pthread
IF ["%TARGET%"]==["omsimulator"] GOTO omsimulator
IF ["%TARGET%"]==["all"] GOTO all
ECHO # Error: No rule to make target '%TARGET%'.
EXIT /B 1


:: -- clean ---------------------------
:clean
IF EXIST "build\win\" RMDIR /S /Q build\win && IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
IF EXIST "install\win\" RMDIR /S /Q install\win && IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- clean ---------------------------


:: -- config xerces -------------------
:xerces
ECHO # config xerces
IF EXIST "3rdParty\xerces\build\win\" RMDIR /S /Q 3rdParty\xerces\build\win
IF EXIST "3rdParty\xerces\install\win\" RMDIR /S /Q 3rdParty\xerces\install\win
MKDIR 3rdParty\xerces\build\win
CD 3rdParty\xerces\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_SHARED_LIBS:BOOL=OFF
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build xerces
msbuild.exe "3rdParty\xerces\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config xerces -------------------


:: -- pthread -------------------------
:pthread
ECHO # config pthread
CD 3rdParty\pthread
START /B /WAIT CMD /C "buildWinVS.bat %OMS_VS_TARGET%"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
ECHO # copy pthread
IF NOT EXIST "install\win\bin" MKDIR install\win\bin
XCOPY /Y /F 3rdParty\pthread\install\win\lib\pthreadVC2.dll install\win\bin
EXIT /B 0
:: -- pthread -------------------------


:: -- config OMSimulator --------------
:omsimulator
ECHO # config OMSimulator
IF EXIST "build\win\" RMDIR /S /Q build\win
MKDIR build\win
cmake.exe -S . -B build\win -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=install\win -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMTLM=OFF -DCMAKE_BUILD_TYPE:STRING=%CMAKE_BUILD_TYPE%
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
EXIT /B 0
:: -- config OMSimulator --------------


:: -- config all ----------------------
:all
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% clean"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
IF NOT EXIST "3rdParty/README.md" GOTO fail2
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% pthread"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% omsimulator"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config all ----------------------


:fail
ECHO Error: Configuring failed for target '%TARGET%'.
EXIT /B 1

:fail2
ECHO Error: Please checkout the 3rdParty submodule, e.g. using 'git submodule update --init 3rdParty', and try again.
EXIT /B 1
