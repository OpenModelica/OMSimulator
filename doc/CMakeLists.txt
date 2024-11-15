
find_package(Doxygen)
find_package(Sphinx)

# If we find either of them then add a custom install target for documentation
if(DOXYGEN_FOUND OR SPHINX_FOUND)
  add_custom_target(install-docs)
endif()

if(DOXYGEN_FOUND)
  add_subdirectory(dev/OMSimulatorLib)
  add_dependencies(install-docs install-doxygen-docs)
  message(STATUS "Doxygen found. Doxygen based documentation generation will be available.")
else()
  message(STATUS "Doxygen not found. Doxygen based documentation generation will NOT be available.")
endif()


if(SPHINX_FOUND)
  add_subdirectory(UsersGuide)
  add_dependencies(install-docs install-sphinx-docs)
  message(STATUS "Sphinx found. Sphinx based documentation generation will be available.")
else()
  message(STATUS "Sphinx not found. Sphinx based documentation generation will NOT be available.")
endif()
