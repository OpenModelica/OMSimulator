project(OMSimulatorServer)

# install only if build as part of the OpenModelica super project
if(DEFINED HOST_SHORT)
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/OMSimulatorServer.py" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OMSimulator/scripts)
endif()
