project(OMSimulatorPython)

set(OMSIMULATORLIB_DIR_STRING ${HOST_SHORT})
IF (WIN32 AND MSVC)
  set(OMSIMULATORLIB_STRING "OMSimulator.dll")
ELSEIF (WIN32 AND MINGW)
  set(OMSIMULATORLIB_STRING "libOMSimulator.dll")
ELSEIF (APPLE)
  set(OMSIMULATORLIB_STRING "libOMSimulator.dylib")
ELSE ()
  set(OMSIMULATORLIB_STRING "libOMSimulator.so")
ENDIF ()

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
