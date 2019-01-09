project(OMSimulator)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(
  ../OMSimulatorLib
  ../OMSysIdentLib
  ../OMSimulatorLua
  ${LUALibrary_INCLUDEDIR})

link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${LUALibrary_LIBRARYDIR}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${MINIZIP_LIBRARYDIR})
IF (OMTLM)
  link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF ()

add_executable(OMSimulator main.cpp)

IF (APPLE)
  add_custom_command(TARGET OMSimulator POST_BUILD COMMAND
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@loader_path/../../../../lib/$(HOST_SHORT)"
    $<TARGET_FILE:OMSimulator>)
  add_custom_command(TARGET OMSimulator POST_BUILD COMMAND
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@loader_path/../lib/$(HOST_SHORT)"
    $<TARGET_FILE:OMSimulator>)
ENDIF ()

IF (Ceres_FOUND AND OMSYSIDENT)
  add_definitions(-DWITH_OMSYSIDENT)
  set(OMSYSIDENT_OPTION OMSysIdent_static)
ENDIF()

IF (WIN32 AND MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF ()

add_definitions(-DOMS_STATIC)
target_link_libraries(OMSimulator OMSimulatorLib_static lua ${OMSYSIDENT_OPTION})

install(TARGETS OMSimulator DESTINATION bin)
