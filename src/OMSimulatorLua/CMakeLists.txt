project(OMSimulatorLua C)

include_directories(
  ../OMSimulatorLib
)

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${OMTLMSIMULATORLIB_LIBRARYDIR})

add_library(OMSimulatorLua STATIC OMSimulatorLua.c)

target_link_libraries(OMSimulatorLua OMSimulatorLib)

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua wsock32 ws2_32 shlwapi)
ENDIF ()

install(TARGETS OMSimulatorLua)
