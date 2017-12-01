project(OMSimulatorPython)

install(FILES OMSimulator.py DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)

IF(WIN32)
  install(FILES OMSimulatorPython.bat DESTINATION ${CMAKE_SOURCE_DIR}/install/bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
ELSEIF(APPLE)
  message(WARNING "OMSimulatorPython is not available on Mac OS X.")
ELSE()
  install(FILES OMSimulatorPython DESTINATION ${CMAKE_SOURCE_DIR}/install/bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
ENDIF()
