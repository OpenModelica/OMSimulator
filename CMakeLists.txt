message("cmake version is: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.14)

project(OMSimulator)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake/")


### Standards ###############################################################################################
## Set the C++ standard to use.
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
## Make sure we do not start relying on extensions down the road.
set(CMAKE_CXX_EXTENSIONS OFF)

## If OMSimulator is NOT being built as part of OpenModelica
## include the required settings here.
if(NOT OPENMODELICA_SUPERPROJECT)
  include(OMSimulatorTopLevelSettings)
endif()

## If OMSimulator is being built as part of OpenModelica
## set the install component to 'omsimulator' for all tagets built by it.
if(NOT OPENMODELICA_SUPERPROJECT)
  set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME omsimulator)
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

message(STATUS "OMSimulator version string: ${OMS_VERSION_STRING}")

IF(MSVC)
  set(PLATFORM_STRING "win")
ELSEIF(MINGW)
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
# Add project modules
add_subdirectory(include)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)

option(OM_OMS_ENABLE_PIP "create the OMSimulator pip package" OFF)
if(OM_OMS_ENABLE_PIP)
  add_subdirectory(src/pip)
endif()

add_subdirectory(doc)

## Add the testsuite directory (which adds the target testsuite-depends which in turn depends
## on omc-diff and testssuite-resources) only if OMSimulator is being used standalone.
option(OM_OMS_ENABLE_TESTSUITE "enable the OMSimulator testsuite" OFF)
if(OM_OMS_ENABLE_TESTSUITE)
  add_subdirectory(testsuite)
endif()
