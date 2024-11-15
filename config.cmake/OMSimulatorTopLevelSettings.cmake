# Build all libraries within OMSimulator with position independent
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find and use ccache if available.
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache. It will be used for compilation C/C++ sources")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
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

# Install libraries to lib/<arch> folder within the installation prefix directory.
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}")

# Install headers to "include/" folder within the installation prefix directory.
# set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/")

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




# Don't allow in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "No in-source builds supported. Change to 'build' sub-directory and do 'cmake ..'.")
endif()
