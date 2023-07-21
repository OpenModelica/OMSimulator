@ECHO off
REM run this on wsl using:
REM cmd.exe /C "configWinVS.bat VS15-Win64"

SET OMS_VS_TARGET=%~1
SET TARGET=%~2
IF NOT DEFINED TARGET SET TARGET=all

echo # Target: %TARGET%

IF ["%OMS_VS_TARGET%"]==["VS14-Win32"] SET OMS_VS_PLATFORM=32&& SET OMS_VS_VERSION="Visual Studio 14 2015"
IF ["%OMS_VS_TARGET%"]==["VS14-Win64"] SET OMS_VS_PLATFORM=64&& SET OMS_VS_VERSION="Visual Studio 14 2015 Win64"
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] SET OMS_VS_PLATFORM=32&& SET OMS_VS_VERSION="Visual Studio 15 2017"
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] SET OMS_VS_PLATFORM=64&& SET OMS_VS_VERSION="Visual Studio 15 2017 Win64"

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
IF ["%OMS_VS_TARGET%"]==["VS15-Win32"] (
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
  ) ELSE (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
  )
)
IF ["%OMS_VS_TARGET%"]==["VS15-Win64"] (
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
  ) ELSE (
    @CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
  )
)


IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE="Release"
IF NOT DEFINED BOOST_ROOT SET BOOST_ROOT=C:\local\boost_1_64_0

ECHO.
SET CMAKE_BUILD_TYPE
SET BOOST_ROOT
ECHO.
:: -- init ----------------------------

IF NOT EXIST build\\ mkdir build
IF NOT EXIST build\\win\\ mkdir build\\win
IF NOT EXIST install\\ MKDIR install
IF NOT EXIST install\\win MKDIR install\\win
IF NOT EXIST install\\win\\lib MKDIR install\\win\\lib

@REM do not change the order to be consistent with line 265 config all
IF ["%TARGET%"]==["clean"] GOTO clean
IF ["%TARGET%"]==["zlib"] GOTO zlib
IF ["%TARGET%"]==["minizip"] GOTO minizip
IF ["%TARGET%"]==["fmi4c"] GOTO fmi4c
IF ["%TARGET%"]==["lua"] GOTO lua
IF ["%TARGET%"]==["cvode"] GOTO cvode
IF ["%TARGET%"]==["kinsol"] GOTO kinsol
IF ["%TARGET%"]==["xerces"] GOTO xerces
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


:: -- config fmi4c ---------------------
:fmi4c
ECHO # config fmi4c
IF EXIST "3rdParty\fmi4c\build\win\" RMDIR /S /Q 3rdParty\fmi4c\build\win
IF EXIST "3rdParty\fmi4c\install\win\" RMDIR /S /Q 3rdParty\fmi4c\install\win
MKDIR 3rdParty\fmi4c\build\win
CD 3rdParty\fmi4c\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DFMI4C_BUILD_SHARED=OFF -DFMI4C_USE_INCLUDED_ZLIB=OFF -DOMS_MINIZIP_INCLUDE_DIR=..\minizip\install\win\include -DOMS_MINIZIP_LIBRARY=..\minizip\install\win\lib\minizip.lib -DOMS_ZLIB_INCLUDE_DIR=..\zlib\install\win\include -DOMS_ZLIB_LIBRARY=..\zlib\install\win\lib\zlibstatic.lib
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build fmi4c
msbuild.exe "3rdParty\fmi4c\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config fmi4c ---------------------


:: -- build Lua ----------------------
:Lua
ECHO # config Lua
IF EXIST "3rdParty\Lua\build\win\" RMDIR /S /Q 3rdParty\Lua\build\win
IF EXIST "3rdParty\Lua\install\win\" RMDIR /S /Q 3rdParty\Lua\install\win
MKDIR 3rdParty\Lua\build\win
CD 3rdParty\Lua\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DLUA_ENABLE_SHARED=OFF -DLUA_ENABLE_TESTING=OFF
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build Lua
msbuild.exe "3rdParty\Lua\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- build Lua ----------------------


:: -- build minizip ----------------------
:minizip
ECHO # config minizip
IF EXIST "3rdParty\minizip\build\win\" RMDIR /S /Q 3rdParty\minizip\build\win
IF EXIST "3rdParty\minizip\install\win\" RMDIR /S /Q 3rdParty\minizip\install\win
MKDIR 3rdParty\minizip\build\win
CD 3rdParty\minizip\build\win
cmake.exe -G %OMS_VS_VERSION% ..\..\src\ -DCMAKE_INSTALL_PREFIX=..\..\install\win
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build minizip
msbuild.exe "3rdParty\minizip\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- build minizip ----------------------

:: -- build zlib ----------------------
:zlib
ECHO # config zlib
IF EXIST "3rdParty\zlib\build\win\" RMDIR /S /Q 3rdParty\zlib\build\win
IF EXIST "3rdParty\zlib\install\win\" RMDIR /S /Q 3rdParty\zlib\install\win
MKDIR 3rdParty\zlib\build\win
CD 3rdParty\zlib\build\win
cmake.exe -G %OMS_VS_VERSION% ..\..\ -DCMAKE_INSTALL_PREFIX=..\..\install\win
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build zlib
msbuild.exe "3rdParty\zlib\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- build zlib ----------------------

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
msbuild.exe "3rdParty\cvode\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
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
msbuild.exe "3rdParty\kinsol\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config kinsol -------------------


:: -- config xerces -------------------
:xerces
ECHO # config xerces
IF EXIST "3rdParty\xerces\build\win\" RMDIR /S /Q 3rdParty\xerces\build\win
IF EXIST "3rdParty\xerces\install\win\" RMDIR /S /Q 3rdParty\xerces\install\win
MKDIR 3rdParty\xerces\build\win
CD 3rdParty\xerces\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_SHARED_LIBS:BOOL=OFF
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build xerces
msbuild.exe "3rdParty\xerces\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config xerces -------------------


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
IF EXIST "3rdParty\libxml2\build\win\" RMDIR /S /Q 3rdParty\libxml2\build\win
IF EXIST "3rdParty\libxml2\install\win\" RMDIR /S /Q 3rdParty\libxml2\install\win
MKDIR 3rdParty\libxml2\build\win
CD 3rdParty\libxml2\build\win
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_INSTALL_PREFIX=..\..\install\win -DBUILD_SHARED_LIBS=OFF -DLIBXML2_WITH_PYTHON=OFF -DLIBXML2_WITH_ZLIB=OFF -DLIBXML2_WITH_LZMA=OFF -DLIBXML2_WITH_TESTS=OFF -DLIBXML2_WITH_ICONV=OFF
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..\..\..
ECHO # build libxml2
msbuild.exe "3rdParty\libxml2\build\win\INSTALL.vcxproj" /t:Build /p:configuration=Release /maxcpucount
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
EXIT /B 0
:: -- config libxml2 ------------------


:: -- copy boost ----------------------
:boost
ECHO # copy boost
IF NOT EXIST "install\win\bin" MKDIR install\win\bin
SET CRD=%CD%
CD %BOOST_ROOT%
FOR /d %%d in (lib%OMS_VS_PLATFORM%-msvc-*) do (
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
cmake.exe -G %OMS_VS_VERSION% ..\.. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE:STRING=%CMAKE_BUILD_TYPE%
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
CD ..\..
EXIT /B 0
:: -- config OMSimulator --------------


:: -- config all ----------------------
:all
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% clean"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
IF NOT EXIST "3rdParty/README.md" GOTO fail2
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% zlib"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% minizip"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% fmi4c"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% lua"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% cvode"
IF NOT ["%ERRORLEVEL%"]==["0"] GOTO fail
START /B /WAIT CMD /C "%~0 %OMS_VS_TARGET% kinsol"
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

:fail2
ECHO Error: Please checkout the 3rdParty submodule, e.g. using 'git submodule update --init 3rdParty', and try again.
EXIT /B 1
