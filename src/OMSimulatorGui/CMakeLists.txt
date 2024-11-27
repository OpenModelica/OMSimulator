project(OMSimulatorGui)

add_executable(OMSimulatorGui main.cpp Application.cpp FPSCounter.cpp)

if(WIN32)
  if(MINGW)
    target_link_options(OMSimulatorGui PRIVATE "-mwindows")
  else()
    target_link_options(OMSimulatorGui PRIVATE "/SUBSYSTEM:WINDOWS")
  endif()
endif()

target_include_directories(OMSimulatorGui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../OMSimulatorLib)
target_link_libraries(OMSimulatorGui PUBLIC OMSimulatorLib_static oms::3rd::GLFW oms::3rd::imgui oms::3rd::tinyfd)

install(TARGETS OMSimulatorGui DESTINATION bin)
