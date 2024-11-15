project(OMSimulator)

add_executable(OMSimulator main.cpp)

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static)

install(TARGETS OMSimulator DESTINATION bin)
