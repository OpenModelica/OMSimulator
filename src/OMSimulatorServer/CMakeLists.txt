project(OMSimulatorServer)

install(FILES
        OMSimulatorSimulationServer.py
        OMSimulatorGuiServer.py
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/OMSimulator/scripts)
