message("cmake version is: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.14)

project(OMSimulator)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config.cmake/")

## If OMSimulator is being built standalone (not as part of OpenModelica)
## include the required settings here.
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  include(OMSimulatorTopLevelSettings)
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
# Add project modules
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)
add_subdirectory(src/pip)
