@echo off
REM run this on wsl using "cmd.exe /C configWinVS14.bat"

IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_63_0

echo # config fmil
if exist "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
if exist "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DFMILIB_BUILD_SHARED_LIB:Bool="0" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" ..\..
CD ..\..\..\..
echo # build fmil
@call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # build Lua
CD 3rdParty\Lua
if exist "install\win\" RMDIR /S /Q install\win
call buildWinVS14.bat
CD ..\..

echo # config cvode
if exist "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
if exist "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config kinsol
if exist "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
if exist "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config OMSimulator
if exist "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..\.. -DBOOST_ROOT=%BOOST_ROOT%
CD ..\..

REM create install\win\bin folder
IF NOT EXIST "install\win\bin" echo # create install\win\bin folder
IF NOT EXIST "install\win\bin" MKDIR "install\win\bin"

echo # copy boost
set CRD=%CD%
cd %BOOST_ROOT%
for /d %%d in (lib64-msvc-*) do (
  cd %%d
  for /r %%e in (boost_system*,boost_filesystem*) do (
     xcopy /Y /F %%e %CRD%\install\win\bin
  )
)
cd %CRD%

echo # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\win\bin
