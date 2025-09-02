project(OMSimulatorLib)

find_package(Threads)

if (ASAN)
  set(CMAKE_BUILD_TYPE Debug)
ENDIF ()

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=uninitialized")
ELSEIF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=uninitialized")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
  if (ASAN)
    message(STATUS "AddressSanitizer enabled")
    add_compile_options("-fsanitize=address" "-fno-omit-frame-pointer")
    link_libraries("-fsanitize=address")
  ENDIF ()
ENDIF ()

set(OMSIMULATORLIB_SOURCES
      AlgLoop.cpp
      BusConnector.cpp
      Clock.cpp
      Clocks.cpp
      Component.cpp
      ComponentFMU3CS.cpp
      ComponentFMUCS.cpp
      ComponentFMUME.cpp
      ComponentTable.cpp
      ComRef.cpp
      Connection.cpp
      Connector.cpp
      CSVReader.cpp
      CSVWriter.cpp
      DirectedGraph.cpp
      Element.cpp
      Flags.cpp
      FMUInfo.cpp
      Logging.cpp
      MatReader.cpp
      MatVer4.cpp
      MATWriter.cpp
      Model.cpp
      OMSFileSystem.cpp
      OMSimulator.cpp
      OMSString.cpp
      ResultReader.cpp
      ResultWriter.cpp
      Scope.cpp
      SignalDerivative.cpp
      Snapshot.cpp
      ssd/ConnectionGeometry.cpp
      ssd/ConnectorGeometry.cpp
      ssd/ElementGeometry.cpp
      ssd/SystemGeometry.cpp
      ssd/Tags.cpp
      System.cpp
      SystemSC.cpp
      SystemWC.cpp
      Values.cpp
      Variable.cpp
      whereami.c
      XercesValidator.cpp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)
list(APPEND OMSIMULATORLIB_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")

# Shared library version
add_library(OMSimulatorLib SHARED ${OMSIMULATORLIB_SOURCES})
set_target_properties(OMSimulatorLib PROPERTIES OUTPUT_NAME OMSimulator)

target_include_directories(OMSimulatorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OMSimulatorLib
                        PUBLIC
                          oms::public_includes
                          oms::3rd::kinsol
                          oms::3rd::cvode
                          oms::3rd::fmi4c
                          oms::3rd::minizip
                          oms::3rd::zlib
                          oms::3rd::lua
                          oms::3rd::pugixml::header
                          oms::3rd::json::header
                          oms::3rd::xerces
                          oms::3rd::ctpl::header)

target_link_libraries(OMSimulatorLib PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

if(MINGW)
  target_link_libraries(OMSimulatorLib PUBLIC shlwapi)
endif()

# Static library version
add_library(OMSimulatorLib_static STATIC ${OMSIMULATORLIB_SOURCES})
if(MSVC)
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator_static)
else()
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator)
endif()

target_compile_definitions(OMSimulatorLib_static PUBLIC OMS_STATIC)

target_include_directories(OMSimulatorLib_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OMSimulatorLib_static
                        PUBLIC
                          oms::public_includes
                          oms::3rd::kinsol
                          oms::3rd::cvode
                          oms::3rd::fmi4c
                          oms::3rd::minizip
                          oms::3rd::zlib
                          oms::3rd::lua
                          oms::3rd::pugixml::header
                          oms::3rd::json::header
                          oms::3rd::xerces
                          oms::3rd::ctpl::header)

target_link_libraries(OMSimulatorLib_static PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

if(MINGW)
  target_link_libraries(OMSimulatorLib_static PUBLIC shlwapi)
endif()

## copy dependent DLL's for mingw artifacts
if(ICU_FOUND)
  message(STATUS "ICU_LIBRARIES_FOUND: ${ICU_LIBRARIES}")
  if (MINGW)
    list(GET ICU_LIBRARIES 0 ICU_PATH)
    get_filename_component(DLL_DIR "${ICU_PATH}/../../bin" ABSOLUTE)
    message(STATUS "DLL_DIR: ${DLL_DIR}")
    # collect all DLLs matching the pattern
    file(GLOB DLL_LIST
              "${DLL_DIR}/libicuuc*.dll"
              "${DLL_DIR}/libicudt*.dll"
              "${DLL_DIR}/libstdc++*.dll"
              "${DLL_DIR}/libwinpthread*.dll"
              "${DLL_DIR}/libgcc_*.dll")
    # Check if the DLL_LIST is not empty before installing
    if (DLL_LIST)
      # Install the files from the list to the 'bin' directory
      install(FILES ${DLL_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
      message(STATUS "Copied files from: ${DLL_LIST} to ${CMAKE_INSTALL_PREFIX}/bin")
    else()
      message(WARNING "No DLLs found matching the pattern")
    endif()
  endif()
endif()

install(TARGETS OMSimulatorLib)
install(TARGETS OMSimulatorLib_static)
