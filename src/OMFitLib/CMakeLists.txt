project(OMFitLib)

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
  ${CMAKE_SOURCE_DIR}/3rdParty/glog/install-win/include
  ${CMAKE_SOURCE_DIR}//3rdParty/gflags/install/win/include
)
link_directories(
  ${FMILibrary_LIBRARYDIR}
  ${Boost_LIBRARY_DIRS}
  ${CMAKE_SOURCE_DIR}/3rdParty/cvode/install/win/lib
  ${CMAKE_SOURCE_DIR}/3rdParty/kinsol/install/win/lib
)

set(OMFITLIB_SOURCES OMFit.cpp FitModel.cpp)
set(OMFITLIB_LIBRARIES OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CERES_LIBRARIES})
# set(OMFITLIB_LIBS2 OMSimulatorLib_shared fmilib_shared sundials_cvode sundials_nvecserial)

# Shared library version
add_library(OMFit_shared SHARED ${OMFITLIB_SOURCES})
set_property(TARGET OMFit_shared PROPERTY CXX_STANDARD 11)
set_target_properties(OMFit_shared PROPERTIES OUTPUT_NAME OMFit)
target_link_libraries(OMFit_shared ${OMFITLIB_LIBRARIES})
install(TARGETS OMFit_shared DESTINATION lib)

# Static library version
add_library(OMFit STATIC ${OMFITLIB_SOURCES})
set_property(TARGET OMFit PROPERTY CXX_STANDARD 11)
set_target_properties(OMFit PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS OMFit DESTINATION lib)

install(FILES OMFit.h DESTINATION include)

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

set(OMFITTEST_LIBRARIES OMFit ${OMFITLIB_LIBRARIES})

add_executable(test_HelloWorld_cs_Fit test_HelloWorld_cs_Fit.c)
target_link_libraries(test_HelloWorld_cs_Fit ${OMFITTEST_LIBRARIES})
add_test(NAME test_HelloWorld_cs_Fit
  COMMAND test_HelloWorld_cs_Fit
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testsuite/OMFit)

add_executable(test_Lin2DimODE_cs_Fit test_Lin2DimODE_cs_Fit.cpp)
target_link_libraries(test_Lin2DimODE_cs_Fit ${OMFITTEST_LIBRARIES})
add_test(NAME test_Lin2DimODE_cs_Fit
  COMMAND test_Lin2DimODE_cs_Fit
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testsuite/OMFit)
