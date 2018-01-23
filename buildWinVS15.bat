@echo off
REM run this on wsl using "cmd.exe /C buildWinVS15.bat"

SET "VSCMD_START_DIR=%CD%"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

echo # build OMSimulator
msbuild.exe "build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release
