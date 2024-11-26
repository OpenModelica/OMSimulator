project(OMSimulator)

add_executable(OMSimulator main.cpp)

if(WIN32)
  target_link_options(OMSimulator PRIVATE "/SUBSYSTEM:WINDOWS")
endif()

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
