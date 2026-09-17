@echo off

set PYTHONPATH=%~dp0;%~dp0\..\lib;%~dp0\..\lib\omc;%PYTHONPATH%
set PATH=%~dp0;%~dp0\..\bin;%PATH%

python -m OMSimulatorGui %*
