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

## On Windows the DLL is a RUNTIME artifact installed under CMAKE_INSTALL_BINDIR
## (plain "bin"), while this package installs under CMAKE_INSTALL_LIBDIR/OMSimulator,
## which nests an extra "<OM_LIBRARY_ARCH>/omc" (see top-level CMakeLists.txt) -- so
## the number of ".." segments between the two depends on that layout and must not be
## hardcoded here (it drifted out of sync with the lib/omc layout once already).
## Compute it instead, so it stays correct however deep CMAKE_INSTALL_LIBDIR nests.
##
## CMAKE_INSTALL_FULL_BINDIR is safe to use as-is: GNUInstallDirs computes it once,
## at include(GNUInstallDirs) time, and nothing touches CMAKE_INSTALL_BINDIR afterwards.
## CMAKE_INSTALL_FULL_LIBDIR is NOT safe: the top-level CMakeLists.txt appends the
## arch/omc suffix onto CMAKE_INSTALL_LIBDIR *after* that include(), so the cached
## FULL_LIBDIR value would still reflect the pre-suffix "lib" -- rebuild that one
## from the final CMAKE_INSTALL_LIBDIR instead of trusting the stale FULL_ variable.
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
  set(_omsimulator_full_libdir "${CMAKE_INSTALL_LIBDIR}")
else()
  set(_omsimulator_full_libdir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

file(RELATIVE_PATH OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR
     "${_omsimulator_full_libdir}/OMSimulator"
     "${CMAKE_INSTALL_FULL_BINDIR}")


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/capi.py" "${CMAKE_CURRENT_BINARY_DIR}/capi.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OMSimulator.in" "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OMSimulator.bat.in" "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator.bat" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/__main__.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/cli.py"
              "${CMAKE_CURRENT_BINARY_DIR}/capi.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/component.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/componenttable.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/connection.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/connector.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/cref.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/dcp.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/elementgeometry.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/enumeration.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/fmu.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/instantiated_model.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/namespace.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/settings.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/ssd.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/ssp.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/ssv.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/ssm.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/system.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/unit.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/utils.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/values.py"
              "${CMAKE_CURRENT_SOURCE_DIR}/variable.py"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/OMSimulator)

if(MSVC)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator.bat" TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                      GROUP_EXECUTE GROUP_READ
                      WORLD_EXECUTE WORLD_READ)
elseif(MINGW)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator" "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator.bat"
          TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                      GROUP_EXECUTE GROUP_READ
                      WORLD_EXECUTE WORLD_READ)
else()
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OMSimulator"
          TYPE BIN
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                      GROUP_EXECUTE GROUP_READ
                      WORLD_EXECUTE WORLD_READ)
endif()
