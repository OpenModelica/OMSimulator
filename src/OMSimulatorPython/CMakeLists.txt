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

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
              "${CMAKE_CURRENT_BINARY_DIR}/capi.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Model.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/NewAPI.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/OMSimulator.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Scope.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/System.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/Types.py"
        TYPE BIN)

if(MSVC)
  install(FILES OMSimulatorPython3.bat TYPE BIN)
  install(DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/OMSimulator
          FILES_MATCHING PATTERN "*boost*mt-1*.dll" PATTERN "OMSimulator" EXCLUDE
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
  install(FILES ${CMAKE_INSTALL_PREFIX}/bin/pthreadVC2.dll
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/OMSimulator
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
elseif(MINGW)
  install(FILES OMSimulatorPython3 OMSimulatorPython3.bat TYPE BIN)
else()
  install(FILES OMSimulatorPython3 TYPE BIN)
endif()
