project(OMSimulatorPython)

IF (WIN32 AND MSVC)
  set(OMSIMULATORLIB_STRING "OMSimulator.dll")
ELSEIF (WIN32 AND MINGW)
  set(OMSIMULATORLIB_STRING "libOMSimulator.dll")
ELSEIF (APPLE)
  set(OMSIMULATORLIB_STRING "libOMSimulator.dylib")
ELSE ()
  set(OMSIMULATORLIB_STRING "libOMSimulator.so")
ENDIF ()

## If OMSimulator is being built as part of OpenModelica and that is set by "OPENMODELICA_NEW_CMAKE_BUILD"
## account for the "omc" directory added in the lib dir (lib/omc/OMSimulator versus lib/OMSimulator)
## currently this sets the path only for windows
## TODO fix the paths for linux
if(OPENMODELICA_NEW_CMAKE_BUILD)
  set(OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR "../../../bin")
else()
  set(OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR "../../bin")
endif()


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/capi.py" "${CMAKE_CURRENT_BINARY_DIR}/capi.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OMSimulatorPython3.in" "${CMAKE_CURRENT_BINARY_DIR}/OMSimulatorPython3" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
              "${CMAKE_CURRENT_BINARY_DIR}/capi.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Model.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/NewAPI.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/OMSimulator.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Scope.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/System.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Types.py"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/OMSimulator)

if(MSVC)
  install(FILES OMSimulatorPython3.bat TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
elseif(MINGW)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OMSimulatorPython3" OMSimulatorPython3.bat
          TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
else()
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OMSimulatorPython3"
          TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
endif()
