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
  # Copy the platform-specific OMSimulator shared library
  install(FILES "$<TARGET_FILE:OMSimulatorLib>" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/install/OMSimulator/")

  find_package(Python COMPONENTS Interpreter REQUIRED)
  add_custom_target(pip_install
    COMMAND "${Python_EXECUTABLE}" -m pip install .
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/install"
    USES_TERMINAL
  )
endif()