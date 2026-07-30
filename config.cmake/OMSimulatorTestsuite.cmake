# Helpers to register OMSimulator testsuite cases as CTest tests.
#
# Each test file carries its own metadata header and its own expected result, so
# registering a test only needs the file name; everything else is handled by
# testsuite/runtest.py.

find_package(Python3 COMPONENTS Interpreter REQUIRED)
include(GNUInstallDirs)

# Must be included from the testsuite directory; subdirectories inherit these.
set(OMS_TESTSUITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(OMS_TESTSUITE_TESTS_ROOT "${OMS_TESTSUITE_ROOT}/tests")
set(OMS_TESTSUITE_DRIVER "${OMS_TESTSUITE_ROOT}/runtest.py")
set(OMS_TESTSUITE_WORK_DIR "${CMAKE_CURRENT_BINARY_DIR}/work")

# The tests run against the installed OMSimulator, because the Python bindings
# are only usable from an install tree.
set(OMS_TESTSUITE_BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
    CACHE PATH "Directory containing the OMSimulator binaries used by the testsuite")

set(OMS_TESTSUITE_TIMEOUT 600
    CACHE STRING "Timeout in seconds for a single testsuite case")

option(OMS_TESTSUITE_ASAN "Run the testsuite in AddressSanitizer mode" OFF)

# Which platform the `## linux: yes` style directives are matched against. The
# driver detects this itself; it only has to be set for a native Windows build
# tested from an MSYS2 shell, which is indistinguishable from a MinGW one.
set(OMS_TESTSUITE_PLATFORM "" CACHE STRING "Platform the tests are enabled for")
set_property(CACHE OMS_TESTSUITE_PLATFORM PROPERTY STRINGS "" linux mac win ucrt64)

set(OMS_TESTSUITE_EXTRA_ARGS "")
if(OMS_TESTSUITE_ASAN)
  list(APPEND OMS_TESTSUITE_EXTRA_ARGS --asan)
endif()
if(OMS_TESTSUITE_PLATFORM)
  list(APPEND OMS_TESTSUITE_EXTRA_ARGS --platform ${OMS_TESTSUITE_PLATFORM})
endif()

# oms_add_test(<file> [<file> ...])
#
# Registers one CTest test per test file, named "<directory>.<file>", e.g.
# "api.NewSSP" or "reference-fmus.2.0.cs.BouncingBall-cs". The directory part is
# also set as a label, so `ctest -L api` runs a single directory.
function(oms_add_test)
  file(RELATIVE_PATH group_path "${OMS_TESTSUITE_TESTS_ROOT}" "${CMAKE_CURRENT_SOURCE_DIR}")
  string(REPLACE "/" "." group "${group_path}")

  foreach(test_file ${ARGN})
    get_filename_component(test_name "${test_file}" NAME_WE)
    set(name "${group}.${test_name}")

    add_test(NAME ${name}
             COMMAND ${Python3_EXECUTABLE} "${OMS_TESTSUITE_DRIVER}"
                     --test "${CMAKE_CURRENT_SOURCE_DIR}/${test_file}"
                     --testsuite-root "${OMS_TESTSUITE_ROOT}"
                     --tests-root "${OMS_TESTSUITE_TESTS_ROOT}"
                     --work-dir "${OMS_TESTSUITE_WORK_DIR}/${name}"
                     --bin-dir "${OMS_TESTSUITE_BIN_DIR}"
                     --omc-diff "$<TARGET_FILE:omc-diff>"
                     ${OMS_TESTSUITE_EXTRA_ARGS})

    set_tests_properties(${name} PROPERTIES
                         LABELS "${group}"
                         SKIP_RETURN_CODE 77
                         TIMEOUT ${OMS_TESTSUITE_TIMEOUT}
                         FIXTURES_REQUIRED oms_testsuite_resources)
  endforeach()
endfunction()
