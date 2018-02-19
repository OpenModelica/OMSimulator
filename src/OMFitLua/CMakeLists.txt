project(OMFitLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN")

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${LUALibrary_INCLUDEDIR}
  ${CMAKE_SOURCE_DIR}/src/OMSimulatorLib
  ${CMAKE_SOURCE_DIR}/src/OMFitLib
)
link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${LUALibrary_LIBRARYDIR}
)

# Shared library version
add_library(OMFitLua SHARED OMFitLua.c)
IF (WIN32 OR APPLE)
  target_link_libraries(OMFitLua OMFit_shared lua)
ELSE ()
  target_link_libraries(OMFitLua OMFit_shared)
ENDIF ()


install(TARGETS OMFitLua DESTINATION lib)
