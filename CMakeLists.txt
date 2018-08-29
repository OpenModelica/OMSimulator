IF(WIN32 AND MSVC)
  # VS 2017 requires Boost 1.64 or newer
  # Boost 1.64 requires CMake 3.8 or newer
  cmake_minimum_required(VERSION 3.8)
ELSE()
  cmake_minimum_required(VERSION 2.8)
ENDIF()

project(OMSimulator)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config.cmake/")
include(GetGitRevisionDescription)
git_describe(GIT_VERSION_STRING --tags --abbrev=7 --match=v*.*)

IF(WIN32 AND MSVC)
  set(PLATFORM_STRING "win")
ELSEIF(WIN32 AND MINGW)
  set(PLATFORM_STRING "mingw")
ELSEIF(APPLE)
  set(PLATFORM_STRING "mac")
ELSE()
  set(PLATFORM_STRING "linux")
ENDIF()

# Don't allow in-source build
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "No in-source builds supported. Change to 'build' sub-directory and do 'cmake ..'.")
ENDIF()

message(STATUS "Platform string: ${PLATFORM_STRING}")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/${PLATFORM_STRING} CACHE PATH "Library installation path (don't change)" FORCE)
ENDIF()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.9.2
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2)
    message(WARNING, "Using boost::regex instead of std::regex due to old gcc compiler")
    set(BOOST_REGEX regex)
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # require at least clang 3.8.0
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.8.0)
    message(WARNING, "Using boost::regex instead of std::regex due to old clang compiler")
    set(BOOST_REGEX regex)
  endif()
endif()

# adrpo: for some strange reson we need -latomic on 32bit Linux
IF("${ABI}" STREQUAL "LINUX32")
    message(WARNING, "Using -latomic for ${ABI}")
	set(LIB_ATOMIC atomic)
ELSE()
    message(STATUS, "ABI is: ${ABI}")
ENDIF("${ABI}" STREQUAL "LINUX32")

# Enable verbose output from Makefile builds
set(CMAKE_VERBOSE_MAKEFILE ON)

# Use C++ 11 for the whole project
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##########################
# Configuring for Boost
IF(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
ENDIF()

find_package(Boost 1.54.0 COMPONENTS system filesystem REQUIRED)
IF(Boost_FOUND)
  message(STATUS "Found Boost")
  message(STATUS "  Boost_LIBRARIES:    " ${Boost_LIBRARIES})
  message(STATUS "  Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
  message(STATUS "  Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS})
ELSE()
  MESSAGE(WARNING, "Boost library not found, please give a hint by setting the cmake variable BOOST_ROOT either in the cmake-gui or the command line, e.g., 'cmake -DBOOST_ROOT=C:/local/boost_1_63_0'")
ENDIF()

##########################
# Configuring for FMILibrary
set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/${PLATFORM_STRING})
find_package(FMILibrary REQUIRED)

##########################
# Configuring for Lua
set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/${PLATFORM_STRING})
find_package(LUALibrary REQUIRED)

##########################
# Configuring for CVODE
set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/${PLATFORM_STRING})
find_package(CVODELibrary REQUIRED)

##########################
# Configuring for KINSOL
set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/${PLATFORM_STRING})
find_package(KINSOLLibrary REQUIRED)

##########################
# Configuring for PUGIXML
add_definitions(-DPUGIXML_HEADER_ONLY)
set(PUGIXML_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/PugiXml)

##########################
# Configuring for CTPL
set(CTPL_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/CTPL)

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
# Configuring for glog
set(GLOG_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/glog/install/${PLATFORM_STRING}/include)

##########################
# Configuring for gflags
set(GFLAGS_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/gflags/install/${PLATFORM_STRING}/include)

##########################
# Configuring for Ceres-Solver
option(OMSYSIDENT "Enable the OMSYSIDENT module for parameter estimation support." ON)
IF(OMSYSIDENT)
find_package(Ceres)
  IF(Ceres_FOUND)
    message(STATUS "Found Ceres-Solver")
    message(STATUS "  CERES_INCLUDE_DIRS: " ${CERES_INCLUDE_DIRS})
    message(STATUS "  GLOG_INCLUDEDIR:    " ${GLOG_INCLUDEDIR})
    message(STATUS "  GFLAGS_INCLUDEDIR:  " ${GFLAGS_INCLUDEDIR})
  ELSE()
    message(WARNING, "Ceres-Solver not found. Building without parameter estimation module.")
  ENDIF()
ENDIF()

##########################
# Configuration for OMTLMSimulatorLib
option(OMTLM "Enable the OMTLMSimulator module." ON)
IF(OMTLM)
set(OMTLMSIMULATOR_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/common)
set(OMTLMSIMULATORLIB_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/common/OMTLMSimulatorLib)
IF(WIN32 AND MSVC)
  set(OMTLMSIMULATORLIB_LIBRARYDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/bin)
ELSE() # use it from the install folder where is properly installed for MacOS
  set(OMTLMSIMULATORLIB_LIBRARYDIR ${CMAKE_INSTALL_PREFIX}/lib/${HOST_SHORT})
ENDIF()
set(OMTLMSIMULATOR_3RDPARTY_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/3rdParty/misc/include)
IF(WIN32)
  set(OMTLM_LINKFLAGS omtlmsimulator wsock32 ws2_32)
ELSE()
  set(OMTLM_LINKFLAGS omtlmsimulator)
ENDIF()
  message(STATUS "OMTLMSimulator enabled.")
ELSE(OMTLM)
  set(OMTLM_LINKFLAGS)
  message(STATUS "OMTLMSimulator disabled.")
ENDIF(OMTLM)

##########################
# Add project modules
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
IF (Ceres_FOUND AND OMSYSIDENT)
  enable_testing()
  add_subdirectory(src/OMSysIdentLib)
ENDIF()
