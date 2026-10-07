project(pip)

if(OMS_ENABLE_LOCAL_PIP)
  set(OMS_PIP_CMDCLASS "")
else()
  set(OMS_PIP_CMDCLASS
"[tool.setuptools.cmdclass]
build_py = \"build_helpers.OMSInstaller\""
  )
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/build_helpers.py" "${CMAKE_CURRENT_BINARY_DIR}/build_helpers.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml" "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/build_helpers.py" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.in" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/)
install(FILES "${CMAKE_SOURCE_DIR}/OSMC-License.txt" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/)

if(OMS_ENABLE_LOCAL_PIP)
  message(STATUS "Local pip package enabled")
  install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/OMSimulator/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/install/OMSimulator/")
  # pyproject.toml lists OMSimulatorGui as a package, so it has to be there.
  install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/OMSimulatorGui/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/install/OMSimulatorGui/"
          PATTERN "__pycache__" EXCLUDE)
  # Copy the platform-specific OMSimulator shared library
  install(FILES "$<TARGET_FILE:OMSimulatorLib>" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/install/OMSimulator/")

  find_package(Python COMPONENTS Interpreter REQUIRED)
  add_custom_target(pip_install
    COMMAND "${Python_EXECUTABLE}" -m pip install .
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/install"
    USES_TERMINAL
  )
endif()

# Source distribution only (no wheels): the sdist carries pyproject.toml,
# build_helpers.py and the license/readme; the binaries are downloaded when the
# package is installed. It is assembled straight from the build tree, so it
# needs neither `cmake --install` nor a compiled OMSimulator.
#   cmake -S . -B build -DOMS_ENABLE_PIP=ON
#   cmake --build build # also: --target pip_sdist -> src/pip/install/dist/*.tar.gz
if(NOT OMS_ENABLE_LOCAL_PIP)
  find_package(Python COMPONENTS Interpreter REQUIRED)
  execute_process(COMMAND "${Python_EXECUTABLE}" -c "import build"
                  RESULT_VARIABLE OMS_PIP_HAVE_BUILD OUTPUT_QUIET ERROR_QUIET)
  if(NOT OMS_PIP_HAVE_BUILD EQUAL 0)
    message(FATAL_ERROR "OMS_ENABLE_PIP needs the Python 'build' module, which is missing from ${Python_EXECUTABLE}. "
                        "Install it (pip install build, or pacman -S mingw-w64-ucrt-x86_64-python-build on MSYS2) "
                        "or select another interpreter with -DPython_EXECUTABLE=<path>.")
  endif()
  set(OMS_PIP_SDIST_DIR "${CMAKE_CURRENT_BINARY_DIR}/sdist")
  # The finished sdist goes next to the local pip package, in src/pip/install/dist.
  set(OMS_PIP_DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/install/dist")
  add_custom_target(pip_sdist ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OMS_PIP_SDIST_DIR}" "${OMS_PIP_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OMS_PIP_SDIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/build_helpers.py"
            "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
            "${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
            "${CMAKE_SOURCE_DIR}/OSMC-License.txt"
            "${OMS_PIP_SDIST_DIR}"
    COMMAND "${Python_EXECUTABLE}" -m build --sdist --outdir "${OMS_PIP_DIST_DIR}" "${OMS_PIP_SDIST_DIR}"
    COMMENT "Building the OMSimulator sdist"
    USES_TERMINAL
  )
endif()
