project(OMSimulator)

add_executable(OMSimulator main.cpp Application.cpp FPSCounter.cpp)

if(MSVC)
  target_link_options(OMSimulator PRIVATE "/SUBSYSTEM:WINDOWS")
endif()

target_link_libraries(OMSimulator PUBLIC OMSimulatorLib_static oms::3rd::GLFW oms::3rd::imgui)

install(TARGETS OMSimulator DESTINATION bin)
