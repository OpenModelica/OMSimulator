project(OMSimulatorLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(
  ${CMAKE_SOURCE_DIR}/src # Needed ?
  ${LUALibrary_INCLUDEDIR}
  ${CMAKE_SOURCE_DIR}/src/OMSimulatorLib
  ${CMAKE_SOURCE_DIR}/src/OMSysIdentLib
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${KINSOLLibrary_LIBRARYDIR})
link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})

IF (Ceres_FOUND AND OMSYSIDENT)
  add_definitions(-DWITH_OMSYSIDENT)
ENDIF()

# Shared library version
add_library(OMSimulatorLua SHARED OMSimulatorLua.c)
IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib sundials_kinsol sundials_cvode sundials_nvecserial lua wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib shlwapi sundials_kinsol sundials_cvode sundials_nvecserial lua wsock32 ws2_32)
ELSEIF (APPLE)
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib sundials_kinsol sundials_cvode sundials_nvecserial lua)
ELSE ()
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib sundials_kinsol sundials_cvode sundials_nvecserial)
ENDIF ()

install(TARGETS OMSimulatorLua DESTINATION lib/${HOST_SHORT})
