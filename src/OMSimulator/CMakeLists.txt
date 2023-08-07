project(OMSimulator)

add_executable(OMSimulator main.cpp)

IF (WIN32 AND MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF ()

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
