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

include_directories(
  ../OMSimulatorLib
  ${FMILibrary_INCLUDEDIR}
  ${CERES_INCLUDE_DIRS}
  ${GLOG_INCLUDEDIR}
  ${GFLAGS_INCLUDEDIR}
)

link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${LUALibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${CVODELibrary_LIBRARYDIR}
  ${KINSOLLibrary_LIBRARYDIR}
  ${OMTLMSIMULATORLIB_LIBRARYDIR}
  ${MINIZIP_LIBRARYDIR}
)

set(OMSYSIDENTLIB_SOURCES OMSysIdent.cpp FitModel.cpp)
set(OMSYSIDENTLIB_LIBRARIES OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial lua ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${OMTLM_LINKFLAGS})

# Static library, to be linked into the OMSimulator executable
add_library(OMSysIdent_static STATIC ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdent_static PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdent_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(OMSysIdent_static PROPERTIES COMPILE_DEFINITIONS "OMS_STATIC")
target_link_libraries(OMSysIdent_static ${CERES_LIBRARIES})

add_library(OMSysIdent SHARED ${OMSYSIDENTLIB_SOURCES})
set_property(TARGET OMSysIdent PROPERTY CXX_STANDARD 11)
set_target_properties(OMSysIdent PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(OMSysIdent ${CERES_LIBRARIES} OMSimulatorLib)

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

set(OMSYSIDENTTEST_LIBRARIES OMSysIdent_static ${OMSYSIDENTLIB_LIBRARIES})

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
