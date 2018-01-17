@echo off
REM run this on wsl using "cmd.exe /C configWinVS14.bat"

IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_63_0

echo # config fmil
if exist "3rdParty\FMIL\build\win\" RMDIR /S /Q 3rdParty\FMIL\build\win
if exist "3rdParty\FMIL\install\win\" RMDIR /S /Q 3rdParty\FMIL\install\win
MKDIR 3rdParty\FMIL\build\win
CD 3rdParty\FMIL\build\win
cmake -G "Visual Studio 14 2015 Win64" -DFMILIB_INSTALL_PREFIX=..\..\install\win -DFMILIB_BUILD_TESTS:BOOL="0" -DFMILIB_GENERATE_DOXYGEN_DOC:BOOL="0" -DFMILIB_BUILD_STATIC_LIB:BOOL="1" -DBUILD_TESTING:BOOL="0" -DFMILIB_BUILD_BEFORE_TESTS:BOOL="0" ..\..
CD ..\..\..\..
echo # build fmil
@call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
msbuild.exe "3rdParty\FMIL\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # build Lua
CD 3rdParty\Lua
if exist "install\win\" RMDIR /S /Q install\win
call buildWinVS14.bat
CD ..\..

echo # config cvode
if exist "3rdParty\cvode\build-win\" RMDIR /S /Q 3rdParty\cvode\build-win
if exist "3rdParty\cvode\install\win\" RMDIR /S /Q 3rdParty\cvode\install\win
MKDIR 3rdParty\cvode\build-win
CD 3rdParty\cvode\build-win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=..\install\win .. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..
echo # build cvode
msbuild.exe "3rdParty\cvode\build-win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config kinsol
if exist "3rdParty\kinsol\build-win\" RMDIR /S /Q 3rdParty\kinsol\build-win
if exist "3rdParty\kinsol\install\win\" RMDIR /S /Q 3rdParty\kinsol\install\win
MKDIR 3rdParty\kinsol\build-win
CD 3rdParty\kinsol\build-win
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=..\install\win .. -DEXAMPLES_ENABLE:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="0"
CD ..\..\..
echo # build kinsol
msbuild.exe "3rdParty\kinsol\build-win\INSTALL.vcxproj" /t:Build /p:configuration=Release

echo # config OMSimulator
if exist "build\win\" RMDIR /S /Q build\win
MKDIR build\win
CD build\win
cmake -G "Visual Studio 14 2015 Win64" ..\.. -DBOOST_ROOT=%BOOST_ROOT%
CD ..\..

echo # create install\bin folder
if exist "install\bin" RMDIR /S /Q install\bin
MKDIR install\bin

echo # copy boost
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_filesystem-vc140-mt-1_63.dll install\bin
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_filesystem-vc140-mt-gd-1_63.dll install\bin
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_system-vc140-mt-1_63.dll install\bin
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_system-vc140-mt-gd-1_63.dll install\bin
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_program_options-vc140-mt-1_63.dll install\bin
COPY %BOOST_ROOT%\lib64-msvc-14.0\boost_program_options-vc140-mt-gd-1_63.dll install\bin

echo # copy fmil
COPY 3rdParty\fmil\install\win\lib\fmilib_shared.dll install\bin

echo # copy lua
COPY 3rdParty\lua\install\win\lua.dll install\bin
COPY 3rdParty\lua\install\win\lua.exe install\bin
