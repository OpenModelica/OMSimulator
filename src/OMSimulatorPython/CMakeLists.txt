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

## If OMSimulator is being built as part of OpenModelica Cmake and that is set by "OPENMODELICA_NEW_CMAKE_BUILD"
## or OMSimulator is being built as part of OpenModelica Makefile  and that is set by "OPENMODELICA_MAKEFILE_BUILD"
## account for the "omc" directory added in the lib dir (lib/omc/OMSimulator versus lib/OMSimulator)
## currently this sets the path only for windows
## TODO fix the paths for linux
if(OPENMODELICA_NEW_CMAKE_BUILD OR "${OPENMODELICA_MAKEFILE_BUILD}" STREQUAL "true")
  set(OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR "../../../bin")
else()
  set(OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR "../../bin")
endif()


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/capi.py" "${CMAKE_CURRENT_BINARY_DIR}/capi.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OMSimulatorPython3.in" "${CMAKE_CURRENT_BINARY_DIR}/OMSimulatorPython3" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
              "${CMAKE_CURRENT_BINARY_DIR}/capi.py"
              "${CMAKE_CURRENT_BINARY_DIR}/connection.py
              "${CMAKE_CURRENT_BINARY_DIR}/connector.py
              "${CMAKE_CURRENT_BINARY_DIR}/fmu.py
              "${CMAKE_CURRENT_BINARY_DIR}/namespace.py
              "${CMAKE_CURRENT_BINARY_DIR}/ssd.py
              "${CMAKE_CURRENT_BINARY_DIR}/ssp.py
              "${CMAKE_CURRENT_BINARY_DIR}/ssv.py
              "${CMAKE_CURRENT_BINARY_DIR}/types.py
              "${CMAKE_CURRENT_BINARY_DIR}/unit.py
              "${CMAKE_CURRENT_BINARY_DIR}/utils.py
              "${CMAKE_CURRENT_BINARY_DIR}/values.py
              "${CMAKE_CURRENT_BINARY_DIR}/variables.py
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
