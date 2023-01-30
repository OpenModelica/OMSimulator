project(OMSimulatorLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(
  ../OMSimulatorLib
)

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${OMTLMSIMULATORLIB_LIBRARYDIR})

# Shared library version
add_library(OMSimulatorLua STATIC OMSimulatorLua.c)

target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib sundials_kinsol_static sundials_cvode_static sundials_nvecserial_static lua_static)
IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua wsock32 ws2_32 shlwapi)
ENDIF ()

install(TARGETS OMSimulatorLua DESTINATION lib/${HOST_SHORT})
