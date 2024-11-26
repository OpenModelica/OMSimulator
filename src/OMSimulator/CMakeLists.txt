project(OMSimulator)

add_executable(OMSimulator main.cpp)

if(WIN32)
  if(MINGW)
    target_link_options(OMSimulator PRIVATE "-mwindows")
  else()
    target_link_options(OMSimulator PRIVATE "/SUBSYSTEM:WINDOWS")
  endif()
endif()

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
