@echo off

set PYTHONPATH=%~dp0;%~dp0\..\lib;%PYTHONPATH%
set PATH=%~dp0;%~dp0\..\bin;%PATH%

python3 %*
