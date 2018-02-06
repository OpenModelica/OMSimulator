@echo off
REM run this on wsl using "cmd.exe /C configWinVS15.bat"

IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_64_0

SET "VSCMD_START_DIR=%CD%"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

echo # config fmil
if exist "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
if exist "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G "Visual Studio 15 2017 Win64" -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DFMILIB_BUILD_SHARED_LIB:Bool="0" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" ..\..
CD ..\..\..\..
echo # build fmil
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # build Lua
CD 3rdParty\Lua
if exist "install\win\" RMDIR /S /Q install\win
call buildWinVS15.bat
CD ..\..

echo # config cvode
if exist "3rdParty\cvode\build\win\" RMDIR /S /Q 3rdParty\cvode\build\win
if exist "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build\win
CD 3rdParty\cvode\build\win
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build cvode
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config kinsol
if exist "3rdParty\kinsol\build\win\" RMDIR /S /Q 3rdParty\kinsol\build\win
if exist "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build\win
CD 3rdParty\kinsol\build\win
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=..\..\install\win ..\.. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..\..
echo # build kinsol
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config OMSimulator
if exist "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G "Visual Studio 15 2017 Win64" ..\.. -DBOOST_ROOT=%BOOST_ROOT%
CD ..\..\

REM create install\win\bin folder
IF NOT EXIST "install\win\bin" echo # create install\win\bin folder
IF NOT EXIST "install\win\bin" MKDIR "install\win\bin"

echo # copy boost
COPY %BOOST_ROOT%\lib64-msvc-*\boost_filesystem*.dll install\win\bin
COPY %BOOST_ROOT%\lib64-msvc-*\boost_system*.dll install\win\bin
COPY %BOOST_ROOT%\lib64-msvc-*\boost_program_options*.dll install\win\bin

echo # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\win\bin
