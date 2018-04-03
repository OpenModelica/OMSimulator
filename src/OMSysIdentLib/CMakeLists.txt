project(OMSysIdentLib)

set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_VERBOSE_MAKEFILE ON)

# Export all symbols from the DLL on Windows
IF (WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  add_definitions(-DBOOST_ALL_DYN_LINK)
ENDIF()

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/OMSimulatorLib
  ${FMILibrary_INCLUDEDIR}
  ${CERES_INCLUDE_DIRS}
  ${GLOG_INCLUDEDIR}
  ${GFLAGS_INCLUDEDIR}
)
link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${OMTLMSIMULATORLIB_LIBRARYDIR}
)

set(OMSYSIDENTLIB_SOURCES OMSysIdent.cpp FitModel.cpp)
set(OMSYSIDENTLIB_LIBRARIES OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CERES_LIBRARIES} omtlmsimulator)
# set(OMSYSIDENTLIB_LIBS2 OMSimulatorLib_shared fmilib_shared sundials_cvode sundials_nvecserial)

# Shared library version
add_library(OMSysIdent_shared SHARED ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdent_shared PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdent_shared PROPERTIES OUTPUT_NAME OMSysIdent)
target_link_libraries(OMSysIdent_shared ${OMSYSIDENTLIB_LIBRARIES})
install(TARGETS OMSysIdent_shared DESTINATION lib)
install(TARGETS OMSysIdent_shared DESTINATION bin)

# Static library version
add_library(OMSysIdent STATIC ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdent PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdent PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS OMSysIdent DESTINATION lib)

install(FILES OMSysIdent.h DESTINATION include)

#############
# Testing
enable_testing()
# For testing use either
# CTEST_OUTPUT_ON_FAILURE=1 make test
# for showing any output from a test program whenever a test fails
# or
# ctest -V
# for showing the stdout output for all test cases, alternatively, for a specific test,
# make test ARGS="-V -R some-specific-test"

set(OMSYSIDENTTEST_LIBRARIES OMSysIdent ${OMSYSIDENTLIB_LIBRARIES})

add_executable(test_HelloWorld_cs_Fit test_HelloWorld_cs_Fit.c)
target_link_libraries(test_HelloWorld_cs_Fit ${OMSYSIDENTTEST_LIBRARIES})
add_test(NAME test_HelloWorld_cs_Fit
  COMMAND test_HelloWorld_cs_Fit
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testsuite/OMSysIdent)

add_executable(test_Lin2DimODE_cs_Fit test_Lin2DimODE_cs_Fit.cpp)
target_link_libraries(test_Lin2DimODE_cs_Fit ${OMSYSIDENTTEST_LIBRARIES})
add_test(NAME test_Lin2DimODE_cs_Fit
  COMMAND test_Lin2DimODE_cs_Fit
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testsuite/OMSysIdent)
