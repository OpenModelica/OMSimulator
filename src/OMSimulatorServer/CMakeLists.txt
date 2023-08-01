project(OMSimulatorServer)

# install only if build as part of the OpenModelica super project
if(NOT OMSIMULATOR_STANDALONE)
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/OMSimulatorServer.py" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OMSimulator/scripts)
endif()
