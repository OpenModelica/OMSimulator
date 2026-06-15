project(OMSimulatorServer)

install(FILES
        OMSimulatorServer.py
        OMSimulatorGuiServer.py
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/OMSimulator/scripts)
