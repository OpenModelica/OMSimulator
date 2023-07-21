message("cmake version is: ${CMAKE_VERSION}")

IF(WIN32 AND MSVC)
  # VS 2017 requires Boost 1.64 or newer
  # Boost 1.64 requires CMake 3.8 or newer
  cmake_minimum_required(VERSION 3.8)
ELSE()
  cmake_minimum_required(VERSION 3.14)
ENDIF()

project(OMSimulator)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache. It will be used for compilation C/C++ sources")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
# else()
#   message(FATAL_ERROR "ccache program not found. It is highly recommended to use ccache for compilation of OpenModelica. Please install ccache and rerun configuration.
#           You can disable this check by specifying -DOM_USE_CCACHE=OFF to CMake configuration command.")
endif()

### Installation configurations ###############################################################################
# Install following GNU conventions (i.e., bin, lib, include, share ...)
include(GNUInstallDirs)

# Precaution so that users do not install in system folders (e.g. /user/, /usr/local/)
# unintentionally. If the user has not specified anything default to an install_cmake dir in the root
# build directory.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install_cmake" CACHE PATH "Default installation directory" FORCE)
    # Prevent sub-projects from changing it by checking CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT after this
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT FALSE)
    message(WARNING "No installation directory specified. Defaulting to: ${CMAKE_INSTALL_PREFIX}")
endif()
# omc_add_to_report(CMAKE_INSTALL_PREFIX)

# We prefer to install libs to "<library architecture>/omc" dir inside lib directory (e.g lib/x86_64-linux-gnu/omc/).
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/omc/")

# We prefer to install headers to "include/omc" dir inside.
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/")

# On macOS we want BUNDLEs (.app) to go to an 'Applications/' directory instead of a 'bin/' directory
if(APPLE)
  set(OM_MACOS_INSTALL_BUNDLEDIR "Applications/")
endif()

## Set the adjusted installation lib directory as an rpath for all installed binaries.
## Assumes binaries end up in either 'bin' or 'lib' AND also assumes that 'bin' and 'lib' are sibling directories.
if(APPLE)
  ## MacOS Bundles end up in bin/<BundleName>.app/Contents/MacOS/
  ## So make sure that the lib dir is referenced from that location as well
  set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR};@loader_path/../../../../${CMAKE_INSTALL_LIBDIR}")
else()
  set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

## Do not print unnecessary install messages when nothing has actually changed.
set(CMAKE_INSTALL_MESSAGE LAZY)










list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config.cmake/")
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

# Don't allow in-source build
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "No in-source builds supported. Change to 'build' sub-directory and do 'cmake ..'.")
ENDIF()

message(STATUS "Platform string: ${PLATFORM_STRING}")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install/${PLATFORM_STRING} CACHE PATH "Library installation path (don't change)" FORCE)
ENDIF()

if("${STD_REGEX}" STREQUAL "0")
    message(WARNING, "Using boost::regex instead of std::regex due to old gcc/clang compiler")
    set(LIB_BOOST_REGEX regex)
else()
    message(STATUS "Using std::regex, the gcc/clang compiler is good enough")
ENDIF()

# We need -latomic on some Linux versions, so let's link it on all Linux to make sure it works
IF("${ABI}" MATCHES "^LINUX")
    find_package(GccAtomic)
    IF (GCCLIBATOMIC_FOUND)
      set(LIB_ATOMIC "${GCCLIBATOMIC_LIBRARY}")
      set(LIB_BOOST_ATOMIC atomic)
      message(STATUS "Using ${LIB_ATOMIC} -lboost_atomic for ${ABI}")
    ENDIF (GCCLIBATOMIC_FOUND)
ELSE()
    message(STATUS "ABI is: ${ABI}")
ENDIF("${ABI}" MATCHES "^LINUX")

# Enable verbose output from Makefile builds
set(CMAKE_VERBOSE_MAKEFILE ON)

# Use C++ 11 for the whole project
# cmake < 3.1 doesn't know or respect CMAKE_CXX_STANDARD*
set(CPP_FS_LIBS "")
IF (CMAKE_VERSION VERSION_LESS 3.1.0)
  IF (CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)[+][+]17")
    set(CPP_FS_LIBS "-lstdc++fs")
    message(STATUS "We have C++17 in CXXFLAGS")
  ELSEIF (CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1)
    set(CMAKE_CXX11_STANDARD_COMPILE_OPTION "-std=c++17")
    set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=gnu++17")
    set(CPP_FS_LIBS "-lstdc++fs")
    message(STATUS "We have C++17 in CXXFLAGS")
  ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_definitions(-DWITHOUT_FS)
    message(STATUS "Using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS} for setting -std=c++11 without checking if it is supported (old cmake version)")
  ENDIF ()
ELSE() # use the freaking CMAKE_CXX_STANDARD*
  IF (CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)[+][+]17")
    set(CMAKE_CXX11_STANDARD_COMPILE_OPTION "")
    set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "")
    set(CPP_FS_LIBS "-lstdc++fs")
    message(STATUS "Clearing CMAKE_CXX11_STANDARD_COMPILE_OPTION since CMAKE_CXX_FLAGS contained C++17 flags")
  ELSEIF (CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1)
    set(CMAKE_CXX11_STANDARD_COMPILE_OPTION "-std=c++17")
    set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=gnu++17")
    set(CPP_FS_LIBS "-lstdc++fs")
  ELSE()
    add_definitions(-DWITHOUT_FS)
  ENDIF ()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message(STATUS "Using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}, CMAKE_CXX_EXTENSIONS: ${CMAKE_CXX_EXTENSIONS} and CMAKE_CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED} for setting ${CMAKE_CXX11_STANDARD_COMPILE_OPTION}")
ENDIF()

##########################
# Configuring for Boost
if (CPP_FS_LIBS STREQUAL "")
  IF(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
  ENDIF()

  find_package(Boost 1.41.0 COMPONENTS filesystem system ${LIB_BOOST_REGEX} ${LIB_BOOST_ATOMIC} REQUIRED)
  IF(Boost_FOUND)
    message(STATUS "Found Boost")
    message(STATUS "  Boost_LIBRARIES:    " ${Boost_LIBRARIES})
    message(STATUS "  Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
    message(STATUS "  Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS})
  ELSE()
    MESSAGE(WARNING "Boost library not found, please give a hint by setting the cmake variable BOOST_ROOT either in the cmake-gui or the command line, e.g., 'cmake -DBOOST_ROOT=C:/local/boost_1_63_0', or by using C++17")
  ENDIF()
ELSE()
  MESSAGE(STATUS "Using C++17 instead of Boost")
ENDIF()

##########################

add_subdirectory(3rdParty)

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
# Configuration for OMTLMSimulatorLib
option(OMTLM "Enable the OMTLMSimulator module." ON)
IF(OMTLM)
set(OMTLMSIMULATOR_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/common)
set(OMTLMSIMULATORLIB_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/common/OMTLMSimulatorLib)
IF(WIN32)
  set(OMTLMSIMULATORLIB_LIBRARYDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/bin)
ELSE() # use it from the install folder where is properly installed for MacOS
  set(OMTLMSIMULATORLIB_LIBRARYDIR ${CMAKE_INSTALL_PREFIX}/lib/${HOST_SHORT})
ENDIF()
set(OMTLMSIMULATOR_3RDPARTY_INCLUDEDIR ${PROJECT_SOURCE_DIR}/OMTLMSimulator/3rdParty/misc/include)
IF(WIN32)
  IF (MINGW)
    # adrpo: link with the full path to the dll on MINGW
    set(OMTLM_LINKFLAGS ${OMTLMSIMULATORLIB_LIBRARYDIR}/libomtlmsimulator.dll wsock32 ws2_32)
    message(STATUS "OMTLMSimulator enabled: ${OMTLM_LINKFLAGS}")
  ELSE(MINGW)
    set(OMTLM_LINKFLAGS omtlmsimulator wsock32 ws2_32)
  ENDIF(MINGW)
ELSE(WIN32)
  set(OMTLM_LINKFLAGS omtlmsimulator)
ENDIF(WIN32)
  message(STATUS "OMTLMSimulator enabled.")
ELSE(OMTLM)
  set(OMTLM_LINKFLAGS)
  message(STATUS "OMTLMSimulator disabled.")
ENDIF(OMTLM)

##########################
# Add project modules
# add_subdirectory(OMTLMSimulator)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)
add_subdirectory(src/pip)
