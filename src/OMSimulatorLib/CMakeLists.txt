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

IF (OMTLM)
  set(OMTLM_SOURCES "TLM/")
  set(TLM_STRING "")
ELSE ()
  set(OMTLM_SOURCES "DummyTLM/")
  set(TLM_STRING "-notlm")
ENDIF ()

set(OMSIMULATORLIB_SOURCES
      AlgLoop.cpp
      BusConnector.cpp
      Clock.cpp
      Clocks.cpp
      Component.cpp
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
      ExternalModelInfo.cpp
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
      StepSizeConfiguration.cpp
      System.cpp
      SystemSC.cpp
      SystemWC.cpp
      Values.cpp
      Variable.cpp
      ssd/ConnectionGeometry.cpp
      ssd/ConnectorGeometry.cpp
      ssd/ElementGeometry.cpp
      ssd/SystemGeometry.cpp
      ssd/Tags.cpp)

if(OMTLM)
  set(OMSIMULATORLIB_SOURCES ${OMSIMULATORLIB_SOURCES}
        ${OMTLM_SOURCES}/ExternalModel.cpp
        ${OMTLM_SOURCES}/SystemTLM.cpp
        ${OMTLM_SOURCES}/TLMBusConnector.cpp
        ../../OMTLMSimulator/3rdParty/misc/src/Bstring.cc
        ../../OMTLMSimulator/3rdParty/misc/src/coordTransform.cc
        ../../OMTLMSimulator/3rdParty/misc/src/double3.cc
        ../../OMTLMSimulator/3rdParty/misc/src/double33.cc
        ../../OMTLMSimulator/3rdParty/misc/src/double33s.cc
        ../../OMTLMSimulator/3rdParty/misc/src/dsyevc3.cc
        ../../OMTLMSimulator/3rdParty/misc/src/dsyevq3.cc
        ../../OMTLMSimulator/3rdParty/misc/src/dsyevv3.cc
        ../../OMTLMSimulator/3rdParty/misc/src/dsytrd3.cc
        ../../OMTLMSimulator/3rdParty/misc/src/ErrorLog.cc
        ../../OMTLMSimulator/3rdParty/misc/src/tostr.cc
        ../../OMTLMSimulator/common/Communication/TLMClientComm.cc
        ../../OMTLMSimulator/common/Communication/TLMCommUtil.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterface.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterface1D.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterface3D.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignal.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignalInput.cc
        ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignalOutput.cc
        ../../OMTLMSimulator/common/Logging/TLMErrorLog.cc
        ../../OMTLMSimulator/common/Parameters/ComponentParameter.cc
        ../../OMTLMSimulator/common/Plugin/PluginImplementer.cc
        ../../OMTLMSimulator/common/Plugin/TLMPlugin.cc)
endif(OMTLM)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)
list(APPEND OMSIMULATORLIB_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")

include_directories(
  ../../3rdParty/RegEx
  ../OMSimulatorLib
  ../OMSimulatorLua
  ${Boost_INCLUDE_DIRS}
  ${CTPL_INCLUDEDIR}
  ${OMTLM_SOURCES})
IF (OMTLM)
  include_directories(
    ${OMTLMSIMULATORLIB_INCLUDEDIR}
    ${OMTLMSIMULATOR_INCLUDEDIR}
    ${OMTLMSIMULATOR_3RDPARTY_INCLUDEDIR})
ENDIF (OMTLM)

link_directories(
  ${Boost_LIBRARY_DIRS})
IF (OMTLM)
  link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF ()

# Shared library version
add_library(OMSimulatorLib SHARED ${OMSIMULATORLIB_SOURCES})
set_target_properties(OMSimulatorLib PROPERTIES OUTPUT_NAME OMSimulator)

target_include_directories(OMSimulatorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(OMSimulatorLib PUBLIC sundials_kinsol_static sundials_cvode_static fmi4c minizip zlibstatic pugixml_header_only lua_static)

# Static library version
add_library(OMSimulatorLib_static STATIC ${OMSIMULATORLIB_SOURCES})
IF (WIN32 AND MSVC)
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator_static)
ELSE ()
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator)
ENDIF ()

target_compile_definitions(OMSimulatorLib_static PUBLIC OMS_STATIC)
target_include_directories(OMSimulatorLib_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(OMSimulatorLib_static PUBLIC sundials_kinsol_static sundials_cvode_static fmi4c minizip zlibstatic pugixml_header_only lua_static)

IF (WIN32 AND MSVC)
  add_definitions(-DBOOST_ALL_DYN_LINK)
ENDIF ()

target_link_libraries(OMSimulatorLib PUBLIC ${LIB_ATOMIC} ${Boost_LIBRARIES} ${OMTLM_LINKFLAGS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CPP_FS_LIBS})
target_link_libraries(OMSimulatorLib_static PUBLIC ${CMAKE_DL_LIBS} ${LIB_ATOMIC} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OMTLM_LINKFLAGS} ${CPP_FS_LIBS})

IF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLib shlwapi)
  target_link_libraries(OMSimulatorLib_static shlwapi)
ENDIF ()

install(TARGETS OMSimulatorLib)
install(TARGETS OMSimulatorLib_static)
install(FILES OMSimulator.h Types.h TYPE INCLUDE)
