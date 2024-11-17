project(OMSimulator)

add_executable(OMSimulator main.cpp)

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static oms::3rd::GLFW oms::3rd::imgui)

install(TARGETS OMSimulator DESTINATION bin)
