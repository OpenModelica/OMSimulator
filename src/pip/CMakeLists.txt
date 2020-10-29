project(pip)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py" "${CMAKE_CURRENT_BINARY_DIR}/setup.py" @ONLY )

set(SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(PIP_DIRECTORY "${CMAKE_INSTALL_PREFIX}/../pip")
set(PYTHON_EXE "")

find_package(PythonInterp)
IF (PYTHONINTERP_FOUND)
  message(STATUS "Found Python Interpretor")
  message(STATUS "  PYTHON_VERSION:      " ${PYTHON_VERSION_STRING})
  message(STATUS "  PYTHON_EXECUTABLE:   " ${PYTHON_EXECUTABLE})
  set(PYTHON_EXE ${PYTHON_EXECUTABLE})
ENDIF()


function(delete dirname)
  IF (EXISTS ${dirname})
    file(GLOB MY_FILES ${dirname}/*.tar.gz)
    IF (MY_FILES)
      file(REMOVE_RECURSE ${MY_FILES})
    ENDIF()
  ENDIF()
endfunction()

## build source distribution python setup.py sdist if python found
IF (EXISTS ${SETUP_PY} AND PYTHON_EXE)

  ## delete older versions if exist
  delete(${CMAKE_CURRENT_BINARY_DIR}/dist)
  execute_process(COMMAND ${PYTHON_EXE} ${SETUP_PY} sdist WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  IF (EXISTS ${PIP_DIRECTORY})
    ## delete older versions of sdist in install/pip 
    delete(${PIP_DIRECTORY}/dist) 
  ENDIF()
  
  ## copy the source distribution to pip directory
  IF (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/dist)
    message(STATUS "Copying files")
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/dist DESTINATION ${PIP_DIRECTORY})
  ENDIF()

ENDIF()


