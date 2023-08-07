project(OMSimulatorLua C)

add_library(OMSimulatorLua STATIC OMSimulatorLua.c)

target_link_libraries(OMSimulatorLua PUBLIC OMSimulatorLib)

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua PUBLIC wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua PUBLIC wsock32 ws2_32 shlwapi)
ENDIF ()

install(TARGETS OMSimulatorLua)
