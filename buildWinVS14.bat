@echo off
REM run this on wsl using "cmd.exe /C buildWinVS14.bat"

rem Set whether "Debug" or "Release"
IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"

@call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 8.1

echo # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=%CMAKE_BUILD_TYPE%
