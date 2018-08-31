project(OMSimulatorLua C)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(
  ${CMAKE_SOURCE_DIR}/src # Needed ?
  ${LUALibrary_INCLUDEDIR}
  ${CMAKE_SOURCE_DIR}/src/OMSimulatorLib
  ${CMAKE_SOURCE_DIR}/src/OMSysIdentLib
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${KINSOLLibrary_LIBRARYDIR})
link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})

IF (Ceres_FOUND AND OMSYSIDENT)
  add_definitions(-DWITH_OMSYSIDENT)
ENDIF()

# Shared library version
add_library(OMSimulatorLua STATIC OMSimulatorLua.c)
set_target_properties(OMSimulatorLua PROPERTIES POSITION_INDEPENDENT_CODE ON)

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib sundials_kinsol sundials_cvode sundials_nvecserial lua wsock32 ws2_32)
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib shlwapi sundials_kinsol sundials_cvode sundials_nvecserial lua wsock32 ws2_32)
ELSEIF (APPLE)
  target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib sundials_kinsol sundials_cvode sundials_nvecserial lua)
ELSE ()
  target_link_libraries(OMSimulatorLua OMSimulatorLib fmilib sundials_kinsol sundials_cvode sundials_nvecserial)
ENDIF ()

# install the damn thing MacOS style
#IF(APPLE)
#  get_target_property(LN_OMSLIB OMSimulatorLib_shared OUTPUT_NAME)
#  get_target_property(LN_OMSLUALIB OMSimulatorLua_shared OUTPUT_NAME)
#  install(CODE "execute_process(COMMAND install_name_tool -id \"@rpath/lib${LN_OMSLUALIB}.dylib\" \"${CMAKE_INSTALL_PREFIX}/lib/${HOST_SHORT}/lib${LN_OMSLUALIB}.dylib\")")
#  install(CODE "execute_process(COMMAND install_name_tool -change \"lib${LN_OMSLIB}.dylib\" \"@rpath/lib${LN_OMSLIB}.dylib\" \"${CMAKE_INSTALL_PREFIX}/lib/${HOST_SHORT}/lib${LN_OMSLUALIB}.dylib\")")
#ENDIF()
# install(TARGETS OMSimulatorLua DESTINATION lib/${HOST_SHORT})
