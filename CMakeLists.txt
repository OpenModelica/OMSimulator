message("cmake version is: ${CMAKE_VERSION}")

IF(WIN32 AND MSVC)
  # VS 2017 requires Boost 1.64 or newer
  # Boost 1.64 requires CMake 3.8 or newer
  cmake_minimum_required(VERSION 3.8)
ELSE()
  cmake_minimum_required(VERSION 2.8)
ENDIF()

project(OMSimulator)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake/")
include(GetGitRevisionDescription)
git_describe(OMS_VERSION_STRING --tags --abbrev=7 --match=v*.* --exclude=*-dev)
if (NOT OMS_VERSION_STRING)
  if (EXISTS "${CMAKE_SOURCE_DIR}/version.txt")
    file(STRINGS "version.txt" OMS_VERSION_STRING)
  ELSE ()
    set(OMS_VERSION_STRING "unknown")
  ENDIF ()
ELSE ()
  STRING(REGEX REPLACE "(.*)-(.*)-(.*)" "\\1.post\\2-\\3" OMS_VERSION_STRING ${OMS_VERSION_STRING})
ENDIF ()
STRING(REGEX REPLACE "v(.*)-(.*)" "\\1" OMS_SHORT_VERSION_STRING "${OMS_VERSION_STRING}")

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
# -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
# Configuring for zip/minizip
#set(MINIZIP_INCLUDEDIR ${PROJECT_SOURCE_DIR}/3rdParty/minizip/install/${PLATFORM_STRING}/include)
#set(MINIZIP_LIBRARYDIR ${PROJECT_SOURCE_DIR}/3rdParty/minizip/install/${PLATFORM_STRING}/lib)

##########################
# Configuring for FMILibrary
#set(FMILibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/install/${PLATFORM_STRING})
#find_package(FMILibrary REQUIRED)

option(FMILIB_BUILD_TESTS "Build tests" OFF)
option(FMILIB_GENERATE_DOXYGEN_DOC "Generate doxygen doc target" OFF)
option(FMILIB_BUILD_STATIC_LIB "Build the library as static (lib/a)" ON)
option(FMILIB_BUILD_SHARED_LIB "Build the library as shared (dll/so/dylib)" OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdParty/FMIL)

# For now we deal with FMIL's nonsensical structure here. They really need to
# fix up their structure as a whole! It is quite non-conventional.
# They also need to fix their cmake usage. It is their default build system and it is used in
# very contrived and unusual ways.

# We create a top level 'include' directory that matches their include structure when FMIL is installed-to-be-used.
# This is how every library should be. That way when you install the library you just change the include
# path and every include will be resolved as it was during build time.  Ideally it should even be in  'include/FMIL/'
# but that won't match how they install their files right now.
file(MAKE_DIRECTORY ${FMILibrary_BINARY_DIR}/include)
# Do not ask me why the fmilib.h is in the cmake.config directory.
file(COPY ${FMILibrary_SOURCE_DIR}/Config.cmake/fmilib.h DESTINATION ${FMILibrary_BINARY_DIR}/include)

file(COPY ${FMILibrary_SOURCE_DIR}/src/Import/include/FMI DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/src/Import/include/FMI1 DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/src/Import/include/FMI2 DESTINATION ${FMILibrary_BINARY_DIR}/include)

file(COPY ${FMILibrary_SOURCE_DIR}/src/Util/include/FMI DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/src/Util/include/FMI1 DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/src/Util/include/FMI2 DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/src/Util/include/JM DESTINATION ${FMILibrary_BINARY_DIR}/include)

file(COPY ${FMILibrary_SOURCE_DIR}/ThirdParty/FMI/default/FMI1 DESTINATION ${FMILibrary_BINARY_DIR}/include)
file(COPY ${FMILibrary_SOURCE_DIR}/ThirdParty/FMI/default/FMI2 DESTINATION ${FMILibrary_BINARY_DIR}/include)

file(COPY ${FMILibrary_BINARY_DIR}/fmilib_config.h DESTINATION ${FMILibrary_BINARY_DIR}/include)

# We give this new directory as include dir for targets that depend on FMIL.
target_include_directories(fmilib INTERFACE ${FMILibrary_BINARY_DIR}/include)
target_include_directories(minizip INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/FMIL/ThirdParty/Minizip/minizip)

##########################
# Configuring for Lua
#set(LUALibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/Lua/install/${PLATFORM_STRING})
#find_package(LUALibrary REQUIRED)
add_subdirectory(3rdParty/Lua)
target_include_directories(lua_static INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/3rdParty/Lua/lua-5.3.4/include)

##########################
# Configuring for CVODE
#set(CVODELibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/cvode/install/${PLATFORM_STRING})
#find_package(CVODELibrary REQUIRED)
option(EXAMPLES_ENABLED "Disable examples" OFF)
option(EXAMPLES_ENABLE_C "Disable examples" OFF)
add_subdirectory(3rdParty/cvode)
target_include_directories(sundials_cvode_static INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/cvode/include)
target_include_directories(sundials_cvode_static INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/3rdParty/cvode/include)

##########################
# Configuring for KINSOL
#set(KINSOLLibrary_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/install/${PLATFORM_STRING})
#find_package(KINSOLLibrary REQUIRED)
option(EXAMPLES_ENABLED "Disable examples" OFF)
add_subdirectory(3rdParty/kinsol)
target_include_directories(sundials_kinsol_static INTERFACE ${PROJECT_SOURCE_DIR}/3rdParty/kinsol/include)
target_include_directories(sundials_kinsol_static INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/3rdParty/kinsol/include)

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
add_subdirectory(src/OMSimulator)
add_subdirectory(src/OMSimulatorLib)
add_subdirectory(src/OMSimulatorLua)
add_subdirectory(src/OMSimulatorPython)
add_subdirectory(src/OMSimulatorServer)

option(OMSIMULATOR_PIP "Enable pip" ON)
if(OMSIMULATOR_PIP)
  add_subdirectory(src/pip)
ENDIF(OMSIMULATOR_PIP)
