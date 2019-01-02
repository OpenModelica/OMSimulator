project(OMSimulatorLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(
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
add_library(OMSimulatorLua STATIC OMSimulatorLua.c)

target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib sundials_kinsol sundials_cvode sundials_nvecserial)
IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua lua wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua lua wsock32 ws2_32 shlwapi)
ELSEIF (APPLE)
  target_link_libraries(OMSimulatorLua lua)
ENDIF ()
