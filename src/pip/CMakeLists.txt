project(pip)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py" "${CMAKE_CURRENT_BINARY_DIR}/setup.py" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
        TYPE BIN)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md"  "${CMAKE_SOURCE_DIR}/OSMC-License.txt"
        TYPE BIN)
