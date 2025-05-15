message("cmake version is: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.14)

project(OMSimulator)

# Options to enable/disable specific components
option(OMS_ENABLE_OMSimulator "Enable OMSimulator component" ON)
option(OMS_ENABLE_OMSimulatorGui "Enable OMSimulator GUI component" OFF)
option(OMS_ENABLE_PIP "Enable pip component" OFF)
option(OMS_ENABLE_TESTSUITE "Enable the OMSimulator testsuite" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake/")

### Standards ##################################################################

# Set the C++ standard to use.
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define NOMINMAX globally
add_definitions(-DNOMINMAX)

if(OPENMODELICA_NEW_CMAKE_BUILD)
  # If OMSimulator is being built as part of OpenModelica
  # set the install component to 'omsimulator' for all tagets built by it.
  set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME omsimulator)
else()
  # If OMSimulator is NOT being built as part of OpenModelica
  # include the required settings here.
  include(OMSimulatorTopLevelSettings)
endif()

################################################################################

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

################################################################################

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

################################################################################

# Add third-party dependencies
add_subdirectory(3rdParty)

# Add project modules
add_subdirectory(include)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)
add_subdirectory(doc)
add_subdirectory(schema)

if(OMS_ENABLE_OMSimulator)
  add_subdirectory(src/OMSimulator)
endif()

if(OMS_ENABLE_OMSimulatorGui)
  add_subdirectory(src/OMSimulatorGui)
endif()

if(OMS_ENABLE_PIP)
  add_subdirectory(src/pip)
endif()

if(OMS_ENABLE_TESTSUITE)
  add_subdirectory(testsuite)
endif()
