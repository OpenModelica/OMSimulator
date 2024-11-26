project(OMSimulator)

add_executable(OMSimulator main.cpp Application.cpp FPSCounter.cpp)

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static oms::3rd::GLFW oms::3rd::imgui oms::3rd::tinyfd)

install(TARGETS OMSimulator DESTINATION bin)
