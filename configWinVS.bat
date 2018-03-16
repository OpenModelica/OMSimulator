@ECHO off
REM run this on wsl using:
REM cmd.exe /C configWinVS.bat VS15-Win64

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

REM default boost version
IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_64_0

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

SET VSCMD_START_DIR="%CD%"

IF ["%~1"]==["VS14-Win32"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
IF ["%~1"]==["VS14-Win64"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
IF ["%~1"]==["VS15-Win64"] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

ECHO # config fmil
IF EXIST "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
IF EXIST "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DFMILIB_BUILD_SHARED_LIB:Bool="0" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" ..\..
CD ..\..\..\..
ECHO # build fmil
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # build Lua
CD 3rdParty\Lua
IF EXIST "install\win\" RMDIR /S /Q install\win
call buildWinVS.bat "%~1"
CD ..\..

ECHO # config cvode
IF EXIST "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
IF EXIST "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
ECHO # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # config kinsol
IF EXIST "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
IF EXIST "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
ECHO # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # config gflags
IF EXIST "3rdParty\gflags\build\win\" RMDIR /S /Q 3rdParty\gflags\build\win
IF EXIST "3rdParty\gflags\install\win\" RMDIR /S /Q 3rdParty\gflags\install\win
MKDIR 3rdParty\gflags\build\win
CD 3rdParty\gflags\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\..\gflags -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..\..
ECHO # build gflags
msbuild.exe "3rdParty\gflags\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # config glog
IF EXIST "3rdParty\glog\build\win\" RMDIR /S /Q 3rdParty\glog\build\win
IF EXIST "3rdParty\glog\install\win\" RMDIR /S /Q 3rdParty\glog\install\win
MKDIR 3rdParty\glog\build\win
CD 3rdParty\glog\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\..\glog -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..\..
ECHO # build glog
msbuild.exe "3rdParty\glog\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # config ceres-solver
IF EXIST "3rdParty\ceres-solver\build\win\" RMDIR /S /Q 3rdParty\ceres-solver\build\win
IF EXIST "3rdParty\ceres-solver\install\win\" RMDIR /S /Q 3rdParty\ceres-solver\install\win
MKDIR 3rdParty\ceres-solver\build\win
CD 3rdParty\ceres-solver\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\..\ceres-solver -DCXX11="ON" -DEXPORT_BUILD_DIR="on" -DEIGEN_INCLUDE_DIR_HINTS="../../eigen/eigen" -DBUILD_EXAMPLES="OFF" -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..\..
ECHO # build ceres-solver
msbuild.exe "3rdParty\ceres-solver\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail

ECHO # config OMSimulator
IF EXIST "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..\.. -DOMFIT="ON" -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\

REM create install\win\bin folder
IF NOT EXIST "install\win\bin" ECHO # create install\win\bin folder
IF NOT EXIST "install\win\bin" MKDIR "install\win\bin"

ECHO # copy boost using CRAPPY xcopy
SET CRD=%CD%
CD %BOOST_ROOT%
FOR /d %%d in (lib*-msvc-*) do (
  CD %%d
  FOR /r %%e in (boost_system*,boost_filesystem*) do (
     xcopy /Y /F %%e %CRD%\install\win\bin
  )
  CD %CRD%
)

ECHO # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\win\bin

EXIT /b 0

:fail
ECHO Configuring failed!
EXIT /b 1
