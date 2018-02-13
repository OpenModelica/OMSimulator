@echo off
REM run this on wsl using:
REM cmd.exe /C configWinVS.bat VS15-Win64

if ["%~1"]==["VS14-Win64"] SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
if ["%~1"]==["VS14-Win32"] SET OMS_VS_VERSION="Visual Studio 14 2015"
if ["%~1"]==["VS15-Win64"] SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

REM exit if no VS version could be recognized
IF NOT DEFINED OMS_VS_VERSION echo Unsupported VS version.
IF NOT DEFINED OMS_VS_VERSION exit 1

echo Using %OMS_VS_VERSION%

REM default boost version
IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_64_0

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

SET "VSCMD_START_DIR=%CD%"
if ["%~1"]==["VS14-Win64"] @call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
if ["%~1"]==["VS15-Win64"] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

echo # config fmil
if exist "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
if exist "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DFMILIB_BUILD_SHARED_LIB:Bool="0" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" ..\..
CD ..\..\..\..
echo # build fmil
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # build Lua
CD 3rdParty\Lua
if exist "install\win\" RMDIR /S /Q install\win
call buildWinVS.bat "%~1"
CD ..\..

echo # config cvode
if exist "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
if exist "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # config kinsol
if exist "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
if exist "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # config gflags
if exist "3rdParty\gflags\build-win\" RMDIR /S /Q 3rdParty\gflags\build-win
if exist "3rdParty\gflags\install\win\" RMDIR /S /Q 3rdParty\gflags\install\win
MKDIR 3rdParty\gflags\build-win
CD 3rdParty\gflags\build-win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\install\win .. -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..
echo # build gflags
msbuild.exe "3rdParty\gflags\build-win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # config glog
if exist "3rdParty\glog\build-win\" RMDIR /S /Q 3rdParty\glog\build-win
rem Use name "install-win" instead of "install\win" since file "INSTALL" already exists in glog repo
if exist "3rdParty\glog\install-win\" RMDIR /S /Q 3rdParty\glog\install-win
MKDIR 3rdParty\glog\build-win
CD 3rdParty\glog\build-win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\install-win .. -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..
echo # build glog
msbuild.exe "3rdParty\glog\build-win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # config ceres-solver
if exist "3rdParty\ceres-solver\build-win\" RMDIR /S /Q 3rdParty\ceres-solver\build-win
if exist "3rdParty\ceres-solver\install\win\" RMDIR /S /Q 3rdParty\ceres-solver\install\win
MKDIR 3rdParty\ceres-solver\build-win
CD 3rdParty\ceres-solver\build-win
cmake -G %OMS_VS_VERSION% -DCMAKE_INSTALL_PREFIX=..\install\win .. -DCXX11="ON" -DEXPORT_BUILD_DIR="on" -DEIGEN_INCLUDE_DIR_HINTS="../eigen" -DBUILD_EXAMPLES="OFF" -DBUILD_TESTING="OFF" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\..
echo # build ceres-solver
msbuild.exe "3rdParty\ceres-solver\build-win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%

echo # config OMSimulator
if exist "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G %OMS_VS_VERSION% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..\.. -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
CD ..\..\

REM create install\win\bin folder
IF NOT EXIST "install\win\bin" echo # create install\win\bin folder
IF NOT EXIST "install\win\bin" MKDIR "install\win\bin"

echo # copy boost using CRAPPY xcopy
set CRD=%CD%
cd %BOOST_ROOT%
for /d %%d in (lib*-msvc-*) do (
  cd %%d
  for /r %%e in (boost_system*,boost_filesystem*) do (
     xcopy /Y /F %%e %CRD%\install\win\bin
  )
)
cd %CRD%

echo # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\win\bin
