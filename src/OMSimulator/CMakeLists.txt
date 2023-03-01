project(OMSimulator)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(../OMSimulatorLib)

link_directories(
  ${ZLIB_LIBRARYDIR}
  ${FMI4c_LIBRARYDIR}
  ${MINIZIP_LIBRARYDIR}
  ${LUALibrary_LIBRARYDIR}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS})
IF (OMTLM)
  link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF ()

add_executable(OMSimulator main.cpp)

IF (BOOST_ROOT AND MINGW)
  # For some reason the above link_directories is ignored when cross-compiling MinGW64...
  target_link_libraries(OMSimulator "-L${BOOST_ROOT}/lib")
  message(STATUS BOOST_ROOT MinGW64 workaround active)
ENDIF ()

IF (APPLE)
  add_custom_command(TARGET OMSimulator POST_BUILD COMMAND
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@loader_path/../../../../lib/${HOST_SHORT}"
    $<TARGET_FILE:OMSimulator>)
  add_custom_command(TARGET OMSimulator POST_BUILD COMMAND
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@loader_path/../lib/${HOST_SHORT}"
    $<TARGET_FILE:OMSimulator>)
ENDIF ()

IF (WIN32 AND MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF ()

add_definitions(-DOMS_STATIC)
target_link_libraries(OMSimulator OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
