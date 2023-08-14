
add_subdirectory(difftool)
add_subdirectory(resources)

add_custom_target(testsuite-depends
                    DEPENDS testsuite-resources
                    DEPENDS omc-diff)
