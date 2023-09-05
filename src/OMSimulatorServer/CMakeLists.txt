project(OMSimulatorServer)

# install only if build as part of the OpenModelica super project
if(OPENMODELICA_SUPERPROJECT)
  install(FILES OMSimulatorServer.py
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/OMSimulator/scripts)
endif()
