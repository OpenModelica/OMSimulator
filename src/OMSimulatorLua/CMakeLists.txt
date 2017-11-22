project(OMSimulatorLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN")

include_directories(${LUALibrary_INCLUDEDIR} ../OMSimulatorLib)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${KINSOLLibrary_LIBRARYDIR})

# Shared library version
add_library(OMSimulatorLua SHARED OMSimulatorLua.c)
IF (WIN32 OR APPLE)
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib_shared sundials_kinsol sundials_cvode sundials_nvecserial lua)
ELSE ()
  target_link_libraries(OMSimulatorLua OMSimulatorLib_shared fmilib_shared sundials_kinsol sundials_cvode sundials_nvecserial)
ENDIF ()

install(TARGETS OMSimulatorLua DESTINATION lib)
