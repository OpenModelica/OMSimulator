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

# Don't allow in-source build
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "No in-source builds supported. Change to 'build' sub-directory and do 'cmake ..'.")
ENDIF()

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  IF(WIN32 AND MSVC)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/win CACHE PATH "Library installation path (don't change)" FORCE)
  ELSEIF(WIN32 AND MINGW)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/mingw CACHE PATH "Library installation path (don't change)" FORCE)
  ELSEIF(APPLE)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/mac CACHE PATH "Library installation path (don't change)" FORCE)
  ELSE()
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/linux CACHE PATH "Library installation path (don't change)" FORCE)
  ENDIF()
ENDIF()

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
IF(WIN32 AND MSVC)
  set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/win)
ELSEIF(WIN32 AND MINGW)
  set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/mingw)
ELSEIF(APPLE)
  set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/mac)
ELSE()
  set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/linux)
ENDIF()
find_package(FMILibrary REQUIRED)

##########################
# Configuring for Lua
IF(WIN32 AND MSVC)
  set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/win)
ELSEIF(WIN32 AND MINGW)
  set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/mingw)
ELSEIF(APPLE)
  set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/mac)
ELSE()
  set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/linux)
ENDIF()
find_package(LUALibrary REQUIRED)

##########################
# Configuring for CVODE
IF(WIN32 AND MSVC)
  set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/win)
ELSEIF(WIN32 AND MINGW)
  set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/mingw)
ELSEIF(APPLE)
  set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/mac)
ELSE()
  set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/linux)
ENDIF()
find_package(CVODELibrary REQUIRED)

##########################
# Configuring for KINSOL
IF(WIN32 AND MSVC)
  set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/win)
ELSEIF(WIN32 AND MINGW)
  set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/mingw)
ELSEIF(APPLE)
  set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/mac)
ELSE()
  set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/linux)
ENDIF()
find_package(KINSOLLibrary REQUIRED)

##########################
# Configuring for PUGIXML
set(PUGIXML_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/PugiXml)

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
IF(WIN32 AND MSVC)
  set(GLOG_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/glog/install/win/include)
ELSEIF(WIN32 AND MINGW)
  set(GLOG_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/glog/install/mingw/include)
ELSEIF(APPLE)
  set(GLOG_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/glog/install/mac/include)
ELSE()
  set(GLOG_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/glog/install/linux/include)
ENDIF()

##########################
# Configuring for gflags
IF(WIN32 AND MSVC)
  set(GFLAGS_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/gflags/install/win/include)
ELSEIF(WIN32 AND MINGW)
  set(GFLAGS_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/gflags/install/mingw/include)
ELSEIF(APPLE)
  set(GFLAGS_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/gflags/install/mac/include)
ELSE()
  set(GFLAGS_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/gflags/install/linux/include)
ENDIF()

##########################
# Configuring for Ceres-Solver
option(OMFIT "Enable the OMFIT module for parameter estimation support." ON)
IF(OMFIT)
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
# Add project modules
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
IF (Ceres_FOUND AND OMFIT)
  enable_testing()
  add_subdirectory(src/OMFitLib)
  add_subdirectory(src/OMFitLua)
  add_subdirectory(src/OMFitPython)
ENDIF()
