@ECHO off
REM run this on wsl using:
REM cmd.exe /C configWinVS.bat VS15-Win64

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
IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_64_0
IF NOT DEFINED CERES SET CERES=OFF
IF NOT DEFINED OMFIT SET OMFIT=OFF

ECHO.
SET CMAKE_BUILD_TYPE
SET BOOST_ROOT
SET CERES
SET OMFIT
ECHO.

:: -- config fmil -------------------------------------------------------------
ECHO # config fmil
IF EXIST "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
IF EXIST "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL=0 -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL=0 -DFMILIB_BUILD_STATIC_LIB:BOOL=1 -DFMILIB_BUILD_SHARED_LIB:BOOL=0 -DBUILD_TESTING:BOOL=0 -DFMILIB_BUILD_BEFORE_TESTS:BOOL=0
CD ..\..\..\..
ECHO # build fmil
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
:: -- config fmil -------------------------------------------------------------

:: -- build Lua ---------------------------------------------------------------
ECHO # build Lua
CD 3rdParty\Lua
IF EXIST "install\win\" RMDIR /S /Q install\win
CALL buildWinVS.bat "%OMS_VS_TARGET%"
CD ..\..
:: -- build Lua ---------------------------------------------------------------

:: -- config cvode ------------------------------------------------------------
ECHO # config cvode
IF EXIST "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
IF EXIST "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0
CD ..\..\..\..
ECHO # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
:: -- config cvode ------------------------------------------------------------

:: -- config kinsol -----------------------------------------------------------
ECHO # config kinsol
IF EXIST "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
IF EXIST "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0
CD ..\..\..\..
ECHO # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
:: -- config kinsol -----------------------------------------------------------

:: -- config gflags -----------------------------------------------------------
ECHO # config gflags
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library gflags, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\gflags\build\win\" RMDIR /S /Q 3rdParty\gflags\build\win
  IF EXIST "3rdParty\gflags\install\win\" RMDIR /S /Q 3rdParty\gflags\install\win
  MKDIR 3rdParty\gflags\build\win
  CD 3rdParty\gflags\build\win
  cmake -G %OMS_VS_VERSION% ..\..\gflags -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  CD ..\..\..\..
  ECHO # build gflags
  msbuild.exe "3rdParty\gflags\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
:: -- config gflags -----------------------------------------------------------

:: -- config glog -------------------------------------------------------------
ECHO # config glog
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library glog, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\glog\build\win\" RMDIR /S /Q 3rdParty\glog\build\win
  IF EXIST "3rdParty\glog\install\win\" RMDIR /S /Q 3rdParty\glog\install\win
  MKDIR 3rdParty\glog\build\win
  CD 3rdParty\glog\build\win
  cmake -G %OMS_VS_VERSION% ..\..\glog -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  CD ..\..\..\..
  ECHO # build glog
  msbuild.exe "3rdParty\glog\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
:: -- config glog -------------------------------------------------------------

:: -- config ceres-solver -----------------------------------------------------
ECHO # config ceres-solver
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library ceres-solver, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\ceres-solver\build\win\" RMDIR /S /Q 3rdParty\ceres-solver\build\win
  IF EXIST "3rdParty\ceres-solver\install\win\" RMDIR /S /Q 3rdParty\ceres-solver\install\win
  MKDIR 3rdParty\ceres-solver\build\win
  CD 3rdParty\ceres-solver\build\win
  cmake -G %OMS_VS_VERSION% ..\..\ceres-solver -DCMAKE_INSTALL_PREFIX=..\..\install\win -DCXX11:BOOL=ON -DEXPORT_BUILD_DIR:BOOL=ON -DEIGEN_INCLUDE_DIR_HINTS="../../eigen/eigen" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  CD ..\..\..\..
  ECHO # build ceres-solver
  msbuild.exe "3rdParty\ceres-solver\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
:: -- config ceres-solver -----------------------------------------------------

:: -- config pthread ----------------------------------------------------------
ECHO # config pthread
CD 3rdParty\pthread
CALL buildWinVS.bat
CD ..\..
:: -- config pthread ----------------------------------------------------------

:: -- config libxml2 ----------------------------------------------------------
ECHO # config libxml2
CD 3rdParty\libxml2
CALL buildWinVS.bat
CD ..\..
:: -- config libxml2 ----------------------------------------------------------

:: -- config OMSimulator ------------------------------------------------------
ECHO # config OMSimulator
IF EXIST "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMFIT="%OMFIT%" -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..
:: -- config OMSimulator ------------------------------------------------------

:: -- create install\win\bin folder -------------------------------------------
IF NOT EXIST "install\win\bin" (
  ECHO # create install\win\bin folder
  MKDIR "install\win\bin"
)
:: -- create install\win\bin folder -------------------------------------------

:: -- copy pthread ------------------------------------------------------------
ECHO # copy pthread
XCOPY /Y /F 3rdParty\pthread\install\win\lib\pthreadVC2.dll install\win\bin
:: -- copy pthread ------------------------------------------------------------

:: -- copy libxml2 ------------------------------------------------------------
ECHO # copy libxml2
XCOPY /Y /F 3rdParty\libxml2\install\win\bin\libxml2.dll install\win\bin
:: -- copy libxml2 ------------------------------------------------------------

:: -- copy boost --------------------------------------------------------------
ECHO # copy boost
SET CRD=%CD%
CD %BOOST_ROOT%
FOR /d %%d in (lib%OMS_VS_PLATFORM%*-msvc-*) do (
  CD %%d
  FOR /r %%e in (boost_system*,boost_filesystem*) do (
    XCOPY /Y /F %%e %CRD%\install\win\bin
  )
  CD %BOOST_ROOT%
)
CD %CRD%
:: -- copy boost --------------------------------------------------------------

:: -- copy lua ----------------------------------------------------------------
ECHO # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\win\bin
:: -- copy lua ----------------------------------------------------------------

EXIT /b 0

:fail
ECHO Configuring failed!
EXIT /b 1
