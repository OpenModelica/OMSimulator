project(OMSysIdentLib)

if (ASAN)
  set(CMAKE_BUILD_TYPE Debug)
ENDIF()
set(CMAKE_INSTALL_RPATH "$ORIGIN")

IF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")
  if (ASAN)
    message(STATUS "AddressSanitizer enabled")
    add_compile_options("-fsanitize=address" "-fno-omit-frame-pointer")
    link_libraries("-fsanitize=address")
  ENDIF()
ENDIF()

# Export all symbols from the DLL on Windows
IF (WIN32 AND MSVC)
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
set(OMSYSIDENTLIB_LIBRARIES OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CERES_LIBRARIES} ${OMTLM_LINKFLAGS})
# set(OMSYSIDENTLIB_LIBS2 OMSimulatorLib_shared fmilib_shared sundials_cvode sundials_nvecserial)


# Static library, to be linked into the OMSimulator executable
add_library(OMSysIdent STATIC ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdent PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdent PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS OMSysIdent DESTINATION lib)

# CMake object library, to be linked into the OMSimulatorLib shared library
# (workaround since simply linking the static library into the shared library doesn't work as intended)
add_library(OMSysIdentObj OBJECT ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdentObj PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdentObj PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
