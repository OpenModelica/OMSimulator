project(OMSimulator)

link_directories(${Boost_LIBRARY_DIRS})

add_executable(OMSimulator main.cpp)

IF (BOOST_ROOT AND MINGW)
  # For some reason the above link_directories is ignored when cross-compiling MinGW64...
  target_link_libraries(OMSimulator PUBLIC "-L${BOOST_ROOT}/lib")
  message(STATUS "BOOST_ROOT MinGW64 workaround active")
ENDIF ()

IF (WIN32 AND MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF ()

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
