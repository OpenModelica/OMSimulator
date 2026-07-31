# Code coverage for the OMSimulator C++ sources, enabled with
# -DOMS_ENABLE_COVERAGE=ON.
#
# Only the OMSimulator libraries are instrumented; 3rdParty is left alone. The
# tests run against the installed library, but the counter files (.gcda) are
# written next to the object files in this build tree, so the report is
# generated from here no matter where the tests ran.
#
# Has to be included after the OMSimulator targets have been defined.

if(NOT OMS_ENABLE_COVERAGE)
  return()
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message(FATAL_ERROR "OMS_ENABLE_COVERAGE needs GCC or Clang, but the compiler is "
                      "${CMAKE_CXX_COMPILER_ID}.")
endif()

find_program(GCOVR_EXECUTABLE gcovr)
if(NOT GCOVR_EXECUTABLE)
  message(FATAL_ERROR "OMS_ENABLE_COVERAGE needs gcovr, which was not found. "
                      "Install it with 'pip install gcovr' or your package manager.")
endif()

# Clang writes gcov data, but only its own llvm-cov can read it back.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(_default_gcov "llvm-cov gcov")
else()
  set(_default_gcov "gcov")
endif()
set(OMS_COVERAGE_GCOV "${_default_gcov}" CACHE STRING
    "The gcov tool gcovr uses; has to match the compiler that built the sources")

set(OMS_COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")

# gcovr report title.
set(OMS_COVERAGE_TITLE
    "${CMAKE_SYSTEM_NAME} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} Code Coverage Report"
    CACHE STRING "Title of the generated HTML coverage report")

include(ProcessorCount)
ProcessorCount(_nproc)
if(_nproc EQUAL 0)
  set(_nproc 1)
endif()
set(OMS_COVERAGE_TEST_JOBS ${_nproc} CACHE STRING
    "How many tests the 'coverage' target runs in parallel")

# -O0 so that line coverage is not distorted by inlining, and atomic counters
# because OMSimulator runs the simulation on several threads.
set(_coverage_compile_options --coverage -fprofile-update=atomic -O0 -g)

foreach(target OMSimulatorLib OMSimulatorLib_static)
  if(TARGET ${target})
    target_compile_options(${target} PRIVATE ${_coverage_compile_options})
    target_link_options(${target} PUBLIC --coverage)
  endif()
endforeach()

file(RELATIVE_PATH _report_path "${CMAKE_BINARY_DIR}" "${OMS_COVERAGE_DIR}/index.html")

# Discard the counters of earlier runs, so that a report never mixes results
# from different test selections.
add_custom_target(coverage-reset
                  COMMAND ${CMAKE_COMMAND} -DOMS_COVERAGE_BINARY_DIR=${CMAKE_BINARY_DIR}
                          -P "${CMAKE_CURRENT_LIST_DIR}/OMSimulatorCoverageReset.cmake"
                  COMMENT "Discarding previously collected coverage data")

add_custom_target(coverage-report
                  COMMAND ${CMAKE_COMMAND} -E make_directory "${OMS_COVERAGE_DIR}"
                  COMMAND ${GCOVR_EXECUTABLE}
                          --root "${CMAKE_SOURCE_DIR}"
                          --filter "${CMAKE_SOURCE_DIR}/src/"
                          --filter "${CMAKE_SOURCE_DIR}/include/"
                          --gcov-executable "${OMS_COVERAGE_GCOV}"
                          --exclude-unreachable-branches
                          --exclude-throw-branches
                          --print-summary
                          --html-title "${OMS_COVERAGE_TITLE}"
                          --html-details "${OMS_COVERAGE_DIR}/index.html"
                          --cobertura "${OMS_COVERAGE_DIR}/coverage.xml"
                          --cobertura-pretty
                          "${CMAKE_BINARY_DIR}"
                  COMMENT "Writing the coverage report to ${_report_path}")

# The whole cycle: forget old counters, run the testsuite, write the report.
if(OMS_ENABLE_TESTSUITE)
  add_custom_target(coverage
                    COMMAND ${CMAKE_COMMAND} -DOMS_COVERAGE_BINARY_DIR=${CMAKE_BINARY_DIR}
                            -P "${CMAKE_CURRENT_LIST_DIR}/OMSimulatorCoverageReset.cmake"
                    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}"
                            --parallel ${OMS_COVERAGE_TEST_JOBS} --output-on-failure
                    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target coverage-report
                    USES_TERMINAL
                    COMMENT "Running the testsuite and writing a coverage report")
endif()
