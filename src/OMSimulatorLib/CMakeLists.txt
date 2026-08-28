project(OMSimulatorLib)

find_package(Threads)

# GCC and Clang both work, but they are slightly different
if (ASAN)
  set(CMAKE_BUILD_TYPE Debug)

  if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1) OR
      (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.1))
    message(FATAL_ERROR
            "ASAN=ON needs GCC 5.1 or newer, or Clang 3.1 or newer, but the selected "
            "C++ compiler is ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} "
            "(${CMAKE_CXX_COMPILER}). Configure with a supported compiler, e.g. "
            "-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++, or build without -DASAN=ON.")
  endif ()

  message(STATUS "AddressSanitizer enabled")
  add_compile_options("-fsanitize=address" "-fno-omit-frame-pointer")
  link_libraries("-fsanitize=address")
endif ()

# Be pedantic about potential memory issues
if (MSVC)
  # MSVC doesn't know -Werror=<name>, its warnings only have numbers.
  set(OMS_MSVC_MEMORY_WARNINGS
        4700 # uninitialized local variable used                (-Wuninitialized)
        4701 # potentially uninitialized local variable used    (-Wmaybe-uninitialized)
        4703 # potentially uninitialized local pointer used     (-Wmaybe-uninitialized)
        4715 # not all control paths return a value             (-Wreturn-type)
        4716 # must return a value                              (-Wreturn-type)
        4172 # returning address of local variable or temporary (-Wreturn-local-addr)
     )
  foreach (warning IN LISTS OMS_MSVC_MEMORY_WARNINGS)
    # /w1<n> reports the warning at the default warning level (C4701 and C4703
    # are level 4 and would stay silent otherwise), /we<n> turns it into an error.
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w1${warning} /we${warning}")
  endforeach ()
else ()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=uninitialized")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
endif ()

set(OMSIMULATORLIB_SOURCES
      AlgLoop.cpp
      BusConnector.cpp
      Clock.cpp
      Clocks.cpp
      Component.cpp
      ComponentDCP.cpp
      ComponentFMU3CS.cpp
      ComponentFMUCS.cpp
      ComponentFMU3ME.cpp
      ComponentFMUME.cpp
      ComponentTable.cpp
      ComRef.cpp
      Connection.cpp
      Connector.cpp
      CSVReader.cpp
      CSVWriter.cpp
      DCPInfo.cpp
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
      Variable.cpp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)
list(APPEND OMSIMULATORLIB_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")

# Shared library version
add_library(OMSimulatorLib SHARED ${OMSIMULATORLIB_SOURCES})
set_target_properties(OMSimulatorLib PROPERTIES OUTPUT_NAME OMSimulator)

add_library(dcplib INTERFACE)
# DCPLib headers live in the top-level 3rdParty directory
target_include_directories(dcplib 
                           INTERFACE 
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/core
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/ethernet
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/master
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/slave
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/xml
                           ${CMAKE_SOURCE_DIR}/3rdParty/DCPLib/include/zip
                           ${CMAKE_SOURCE_DIR}/3rdParty/asio/include)
target_link_libraries(OMSimulatorLib PRIVATE dcplib)
if(WIN32)
  target_link_libraries(OMSimulatorLib PUBLIC ws2_32)
endif()

target_include_directories(OMSimulatorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OMSimulatorLib
                        PUBLIC
                          oms::public_includes
                          oms::3rd::sundials::kinsol
                          oms::3rd::sundials::cvode
                          oms::3rd::fmi4c
                          oms::3rd::minizip
                          oms::3rd::zlib
                          oms::3rd::pugixml::header
                          oms::3rd::json::header
                          oms::3rd::ctpl::header
                          oms::3rd::xerces
                          oms::3rd::libzip)

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
                          oms::3rd::sundials::kinsol
                          oms::3rd::sundials::cvode
                          oms::3rd::fmi4c
                          oms::3rd::minizip
                          oms::3rd::zlib
                          oms::3rd::pugixml::header
                          oms::3rd::json::header
                          oms::3rd::ctpl::header
                          oms::3rd::xerces
                          oms::3rd::ctpl::header
                          oms::3rd::libzip)
                          
target_link_libraries(OMSimulatorLib_static PRIVATE dcplib)

target_link_libraries(OMSimulatorLib_static PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
  target_link_libraries(OMSimulatorLib_static PUBLIC ws2_32)
endif()

if(MINGW)
  target_link_libraries(OMSimulatorLib_static PUBLIC shlwapi)
endif()

## copy dependent DLL's for mingw artifacts
if (MINGW)
  # Extract compiler path (g++ path)
  get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

  # MinGW DLLs are usually in the same directory
  set(MINGW_BIN_DIR "${COMPILER_DIR}")

  message(STATUS "Detected MinGW bin directory: ${MINGW_BIN_DIR}")

  file(GLOB DLL_LIST
        "${MINGW_BIN_DIR}/libstdc++*.dll"
        "${MINGW_BIN_DIR}/libwinpthread*.dll"
        "${MINGW_BIN_DIR}/libgcc_*.dll"
  )
  # Check if the DLL_LIST is not empty before installing
  if (DLL_LIST)
    # Install the files from the list to the 'bin' directory
    install(FILES ${DLL_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    message(STATUS "Copied files from: ${DLL_LIST} to ${CMAKE_INSTALL_PREFIX}/bin")
  else()
    message(WARNING "No DLLs found matching the pattern")
  endif()
endif()

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
