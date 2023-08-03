message("cmake version is: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.14)

project(OMSimulator)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config.cmake/")


### Standards ###############################################################################################
## Set the C++ standard to use.
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
## Make sure we do not start relying on extensions down the road.
set(CMAKE_CXX_EXTENSIONS OFF)

## If OMSimulator is being built standalone (not as part of OpenModelica)
## include the required settings here.
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(OMSIMULATOR_STANDALONE ON)
  include(OMSimulatorTopLevelSettings)
else()
  set(OMSIMULATOR_STANDALONE OFF)
endif()


include(GetGitRevisionDescription)
if (EXISTS "${CMAKE_SOURCE_DIR}/version.txt")
  file(STRINGS "version.txt" OMS_VERSION_STRING)
ELSE ()
  git_describe(OMS_VERSION_STRING --tags --abbrev=7 --match=v*.* --exclude=*-dev)
  if (NOT OMS_VERSION_STRING)
    set(OMS_VERSION_STRING "unknown")
  ELSE ()
    STRING(REGEX REPLACE "(.*)-(.*)-(.*)" "\\1.post\\2-\\3" OMS_VERSION_STRING ${OMS_VERSION_STRING})
  ENDIF ()
ENDIF ()
STRING(REGEX REPLACE "v(.*)-(.*)" "\\1" OMS_SHORT_VERSION_STRING "${OMS_VERSION_STRING}")

message("OMSimulator version string: ${OMS_VERSION_STRING}")

IF(WIN32 AND MSVC)
  set(PLATFORM_STRING "win")
ELSEIF(WIN32 AND MINGW)
  set(PLATFORM_STRING "mingw")
ELSEIF(APPLE)
  set(PLATFORM_STRING "mac")
ELSE()
  set(PLATFORM_STRING "linux")
ENDIF()

message(STATUS "Platform string: ${PLATFORM_STRING}")

##########################

add_subdirectory(3rdParty)


##########################
# Configuring for Python
find_package(PythonLibs)
IF(PYTHONLIBS_FOUND)
  message(STATUS "Found Python")
  message(STATUS "  PYTHON_VERSION:      " ${PYTHONLIBS_VERSION_STRING})
  message(STATUS "  PYTHON_LIBRARIES:    " ${PYTHON_LIBRARIES})
  message(STATUS "  PYTHON_INCLUDE_DIRS: " ${PYTHON_INCLUDE_DIRS})
ELSE()
  MESSAGE(WARNING, "Python library not found.")
ENDIF()

##########################
# Add project modules
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)
add_subdirectory(src/pip)
