project(OMSimulatorLib)

find_package(Threads)

if (ASAN)
  set(CMAKE_BUILD_TYPE Debug)
ENDIF ()
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

IF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
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

list(APPEND OMSIMULATORLIB_SOURCES AlgLoopSolver.cpp)
list(APPEND OMSIMULATORLIB_SOURCES BusConnector.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Clock.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Clocks.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Component.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ComponentFMUCS.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ComponentFMUME.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ComponentTable.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ComRef.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Connection.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Connector.cpp)
list(APPEND OMSIMULATORLIB_SOURCES CSVReader.cpp)
list(APPEND OMSIMULATORLIB_SOURCES CSVWriter.cpp)
list(APPEND OMSIMULATORLIB_SOURCES DirectedGraph.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Element.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ExternalModelInfo.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Flags.cpp)
list(APPEND OMSIMULATORLIB_SOURCES FMUInfo.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Logging.cpp)
list(APPEND OMSIMULATORLIB_SOURCES MatReader.cpp)
list(APPEND OMSIMULATORLIB_SOURCES MatVer4.cpp)
list(APPEND OMSIMULATORLIB_SOURCES MATWriter.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Model.cpp)
list(APPEND OMSIMULATORLIB_SOURCES OMSFileSystem.cpp)
list(APPEND OMSIMULATORLIB_SOURCES OMSimulator.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ResultReader.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ResultWriter.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Scope.cpp)
list(APPEND OMSIMULATORLIB_SOURCES SignalDerivative.cpp)
list(APPEND OMSIMULATORLIB_SOURCES StepSizeConfiguration.cpp)
list(APPEND OMSIMULATORLIB_SOURCES System.cpp)
list(APPEND OMSIMULATORLIB_SOURCES SystemSC.cpp)
list(APPEND OMSIMULATORLIB_SOURCES SystemWC.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Values.cpp)
list(APPEND OMSIMULATORLIB_SOURCES Variable.cpp)

list(APPEND OMSIMULATORLIB_SOURCES ssd/ConnectionGeometry.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ssd/ConnectorGeometry.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ssd/ElementGeometry.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ssd/SystemGeometry.cpp)
list(APPEND OMSIMULATORLIB_SOURCES ssd/Tags.cpp)

IF (OMTLM)
  list(APPEND OMSIMULATORLIB_SOURCES ${OMTLM_SOURCES}/ExternalModel.cpp)
  list(APPEND OMSIMULATORLIB_SOURCES ${OMTLM_SOURCES}/SystemTLM.cpp)
  list(APPEND OMSIMULATORLIB_SOURCES ${OMTLM_SOURCES}/TLMBusConnector.cpp)

  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/Bstring.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/coordTransform.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/double3.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/double33.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/double33s.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/dsyevc3.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/dsyevq3.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/dsyevv3.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/dsytrd3.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/ErrorLog.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/3rdParty/misc/src/tostr.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Communication/TLMClientComm.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Communication/TLMCommUtil.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterface.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterface1D.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterface3D.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignal.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignalInput.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Interfaces/TLMInterfaceSignalOutput.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Logging/TLMErrorLog.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Parameters/ComponentParameter.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Plugin/PluginImplementer.cc)
  list(APPEND OMSIMULATORLIB_SOURCES ../../OMTLMSimulator/common/Plugin/TLMPlugin.cc)
ENDIF (OMTLM)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)
list(APPEND OMSIMULATORLIB_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")

include_directories(
  ../../3rdParty/RegEx
  ../OMSimulatorLib
  ../OMSimulatorLua
  ${Boost_INCLUDE_DIRS}
  ${CTPL_INCLUDEDIR}
  ${CVODELibrary_INCLUDEDIR}
  ${FMILibrary_INCLUDEDIR}
  ${KINSOLLibrary_INCLUDEDIR}
  ${LUALibrary_INCLUDEDIR}
  ${MINIZIP_INCLUDEDIR}
  ${OMTLM_SOURCES}
  ${PUGIXML_INCLUDEDIR})
IF (OMTLM)
  include_directories(
    ${OMTLMSIMULATORLIB_INCLUDEDIR}
    ${OMTLMSIMULATOR_INCLUDEDIR}
    ${OMTLMSIMULATOR_3RDPARTY_INCLUDEDIR})
ENDIF (OMTLM)

link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${LUALibrary_LIBRARYDIR}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${MINIZIP_LIBRARYDIR})
IF (OMTLM)
  link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF ()

# Shared library version
add_library(OMSimulatorLib SHARED ${OMSIMULATORLIB_SOURCES})
set_target_properties(OMSimulatorLib PROPERTIES OUTPUT_NAME OMSimulator)
IF (APPLE)
  set_target_properties(OMSimulatorLib PROPERTIES INSTALL_NAME_DIR "@rpath")
ENDIF ()

# Static library version
add_library(OMSimulatorLib_static STATIC ${OMSIMULATORLIB_SOURCES})
IF (WIN32 AND MSVC)
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator_static)
ELSE ()
  set_target_properties(OMSimulatorLib_static PROPERTIES OUTPUT_NAME OMSimulator)
ENDIF ()
set_target_properties(OMSimulatorLib_static PROPERTIES COMPILE_DEFINITIONS "OMS_STATIC")

IF (WIN32 AND MSVC)
  add_definitions(-DBOOST_ALL_DYN_LINK)
ENDIF ()

target_link_libraries(OMSimulatorLib fmilib sundials_kinsol sundials_cvode sundials_nvecserial minizip ${LIB_ATOMIC} ${Boost_LIBRARIES} ${OMTLM_LINKFLAGS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CPP_FS_LIBS} ${STATIC_LIBGCC})
target_link_libraries(OMSimulatorLib_static fmilib sundials_kinsol sundials_cvode sundials_nvecserial minizip lua ${CMAKE_DL_LIBS} ${LIB_ATOMIC} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OMTLM_LINKFLAGS} ${CPP_FS_LIBS} ${STATIC_LIBGCC})

IF (WIN32 AND MINGW)
  target_link_libraries(OMSimulatorLib shlwapi)
  target_link_libraries(OMSimulatorLib_static shlwapi)
ENDIF ()

IF (NOT (WIN32 OR MSVC))
  target_link_libraries(OMSimulatorLib "-static-libstdc++")
  target_link_libraries(OMSimulatorLib_static "-static-libstdc++")
ENDIF ()

IF (WIN32)
  install(TARGETS OMSimulatorLib DESTINATION bin)
ELSE ()
  install(TARGETS OMSimulatorLib DESTINATION lib/${HOST_SHORT})
ENDIF ()

install(TARGETS OMSimulatorLib_static DESTINATION lib/${HOST_SHORT})
install(FILES OMSimulator.h Types.h DESTINATION include)
