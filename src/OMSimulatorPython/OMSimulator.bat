@echo off

set PYTHONPATH=%~dp0;%~dp0\..\lib;%~dp0\..\lib\omc;%PYTHONPATH%
set PATH=%~dp0;%~dp0\..\bin;%PATH%

rem A ".py" argument is run as a driver script, as before. Anything else
rem (e.g. a .ssp or .fmu model file) is handed to the OMSimulator CLI module.
if /I "%~x1"==".py" (
  python %*
) else (
  python -m OMSimulator %*
)
