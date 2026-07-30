
add_subdirectory(difftool)
add_subdirectory(resources)

add_custom_target(testsuite-depends
                    DEPENDS testsuite-resources
                    DEPENDS omc-diff)

include(OMSimulatorTestsuite)

# CTest does not build anything, so the FMUs, SSPs and omc-diff the tests need
# are built by a setup fixture that every test depends on.
add_test(NAME testsuite-depends
         COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}"
                                  --config $<CONFIG>
                                  --target testsuite-depends)
set_tests_properties(testsuite-depends PROPERTIES
                     FIXTURES_SETUP oms_testsuite_resources)

# Re-record the expected results of all tests from their actual output.
# `ctest -R <regex>` cannot be forwarded here, so use the environment variable
# directly to rebaseline a subset:  OMS_TESTSUITE_BASELINE=1 ctest -R api
add_custom_target(testsuite-baseline
                  COMMAND ${CMAKE_COMMAND} -E env OMS_TESTSUITE_BASELINE=1
                          ${CMAKE_CTEST_COMMAND}
                  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                  DEPENDS testsuite-depends
                  COMMENT "Updating the expected results of all testsuite cases")

add_subdirectory(tests)
