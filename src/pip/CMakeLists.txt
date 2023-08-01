project(pip)

set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR}/pip)
set(CMAKE_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/pip)

IF (OMS_VERSION_STRING MATCHES "^(.*)\\.post(.*)-g(.*)$")
  set(OMS_BUILD_TYPE "nightly")
ELSE()
  set(OMS_BUILD_TYPE "stable")
ENDIF()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py" "${CMAKE_CURRENT_BINARY_DIR}/setup.py" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
        TYPE BIN)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md"  "${CMAKE_SOURCE_DIR}/OSMC-License.txt"
        TYPE DATA)
