add_library(OMSimulatorInclude INTERFACE)

target_include_directories(OMSimulatorInclude
                            INTERFACE
                              ${CMAKE_CURRENT_SOURCE_DIR})

add_library(oms::public_includes ALIAS OMSimulatorInclude)
install(DIRECTORY OMSimulator TYPE INCLUDE)
