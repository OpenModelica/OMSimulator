@ECHO off
REM run this on wsl using:
REM cmd.exe /C configWinVS.bat VS15-Win64

SET OMS_VS_TARGET=%~1
SET TARGET=%~2
IF NOT DEFINED TARGET SET TARGET=all

echo # Target: %TARGET%

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

:: -- init ----------------------------
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
:: -- init ----------------------------

IF NOT EXIST build\\ mkdir build
IF NOT EXIST build\\win\\ mkdir build\\win
IF NOT EXIST install\\ MKDIR install
IF NOT EXIST install\\win MKDIR install\\win
IF NOT EXIST install\\win\\lib MKDIR install\\win\\lib

IF ["%TARGET%"]==["clean"] GOTO clean
IF ["%TARGET%"]==["fmil"] GOTO fmil
IF ["%TARGET%"]==["lua"] GOTO lua
IF ["%TARGET%"]==["cvode"] GOTO cvode
IF ["%TARGET%"]==["kinsol"] GOTO kinsol
IF ["%TARGET%"]==["gflags"] GOTO gflags
IF ["%TARGET%"]==["glog"] GOTO glog
IF ["%TARGET%"]==["ceres"] GOTO ceres
IF ["%TARGET%"]==["pthread"] GOTO pthread
IF ["%TARGET%"]==["libxml2"] GOTO libxml2
IF ["%TARGET%"]==["boost"] GOTO boost
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


:: -- config fmil ---------------------
:fmil
ECHO # config fmil
IF EXIST "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
IF EXIST "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL=0 -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL=0 -DFMILIB_BUILD_STATIC_LIB:BOOL=1 -DFMILIB_BUILD_SHARED_LIB:BOOL=0 -DBUILD_TESTING:BOOL=0 -DFMILIB_BUILD_BEFORE_TESTS:BOOL=0
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build fmil
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config fmil ---------------------


:: -- build Lua -----------------------
:lua
ECHO # build Lua
CD 3rdParty\Lua
START /B /WAIT CMD /C "buildWinVS.bat %OMS_VS_TARGET%"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
ECHO # copy lua
IF NOT EXIST "install\win\bin" MKDIR install\win\bin
COPY 3rdParty\lua\install\win\lua.dll install\win\bin
EXIT /B 0
:: -- build Lua -----------------------


:: -- config cvode --------------------
:cvode
ECHO # config cvode
IF EXIST "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
IF EXIST "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config cvode --------------------


:: -- config kinsol -------------------
:kinsol
ECHO # config kinsol
IF EXIST "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
IF EXIST "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DEXAMPLES_ENABLE:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config kinsol -------------------


:: -- config gflags -------------------
:gflags
ECHO # config gflags
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library gflags, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\gflags\build\win\" RMDIR /S /Q 3rdParty\gflags\build\win
  IF EXIST "3rdParty\gflags\install\win\" RMDIR /S /Q 3rdParty\gflags\install\win
  MKDIR 3rdParty\gflags\build\win
  CD 3rdParty\gflags\build\win
  cmake.exe -G %OMS_VS_VERSION% ..\..\gflags -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
  CD ..\..\..\..
  ECHO # build gflags
  msbuild.exe "3rdParty\gflags\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
EXIT /B 0
:: -- config gflags -------------------


:: -- config glog ---------------------
:glog
ECHO # config glog
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library glog, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\glog\build\win\" RMDIR /S /Q 3rdParty\glog\build\win
  IF EXIST "3rdParty\glog\install\win\" RMDIR /S /Q 3rdParty\glog\install\win
  MKDIR 3rdParty\glog\build\win
  CD 3rdParty\glog\build\win
  cmake.exe -G %OMS_VS_VERSION% ..\..\glog -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
  CD ..\..\..\..
  ECHO # build glog
  msbuild.exe "3rdParty\glog\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
EXIT /B 0
:: -- config glog ---------------------


:: -- config ceres-solver -------------
:ceres
ECHO # config ceres-solver
IF ["%CERES%"]==["OFF"] (
  ECHO CERES=OFF: Skipping build of 3rdParty library ceres-solver, which is a dependency of the optional parameter estimation module.
) ELSE (
  IF EXIST "3rdParty\ceres-solver\build\win\" RMDIR /S /Q 3rdParty\ceres-solver\build\win
  IF EXIST "3rdParty\ceres-solver\install\win\" RMDIR /S /Q 3rdParty\ceres-solver\install\win
  MKDIR 3rdParty\ceres-solver\build\win
  CD 3rdParty\ceres-solver\build\win
  cmake.exe -G %OMS_VS_VERSION% ..\..\ceres-solver -DCMAKE_INSTALL_PREFIX=..\..\install\win -DCXX11:BOOL=ON -DEXPORT_BUILD_DIR:BOOL=ON -DEIGEN_INCLUDE_DIR_HINTS="../../eigen/eigen" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
  CD ..\..\..\..
  ECHO # build ceres-solver
  msbuild.exe "3rdParty\ceres-solver\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE% /maxcpucount
  IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
)
EXIT /B 0
:: -- config ceres-solver -------------


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


:: -- config libxml2 ------------------
:libxml2
ECHO # config libxml2
CD 3rdParty\libxml2
START /B /WAIT CMD /C "buildWinVS.bat %OMS_VS_TARGET%"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
ECHO # copy libxml2
IF NOT EXIST "install\win\bin" MKDIR install\win\bin
XCOPY /Y /F 3rdParty\libxml2\install\win\bin\libxml2.dll install\win\bin
EXIT /B 0
:: -- config libxml2 ------------------


:: -- copy boost ----------------------
:boost
ECHO # copy boost
IF NOT EXIST "install\win\bin" MKDIR install\win\bin
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
EXIT /B 0
:: -- copy boost ----------------------


:: -- config OMSimulator --------------
:omsimulator
ECHO # config OMSimulator
IF EXIST "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOMFIT="%OMFIT%" -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
EXIT /B 0
:: -- config OMSimulator --------------


:: -- config all ----------------------
:all
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% clean"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% fmil"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% lua"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% cvode"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% kinsol"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% gflags"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% glog"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% ceres"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% pthread"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% libxml2"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% boost"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% omsimulator"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config all ----------------------


:fail
ECHO Error: Configuring failed for target '%TARGET%'.
EXIT /B 1
