# Install script for directory: /home/lochel/OMSimulator/testsuite/resources/EventTest/sources

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so"
         RPATH "$ORIGIN")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/build/EventTest.so")
  if(EXISTS "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so"
         OLD_RPATH ":::::::"
         NEW_RPATH "$ORIGIN")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/EventTest.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(GET_RUNTIME_DEPENDENCIES
    RESOLVED_DEPENDENCIES_VAR _CMAKE_DEPS
    LIBRARIES
      "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/build/EventTest.so"
    DIRECTORIES
      "C:/OPENMODELICAGIT/OpenModelica/build/bin"
    PRE_EXCLUDE_REGEXES
      "api-ms-"
      "ext-ms-"
    POST_EXCLUDE_REGEXES
      "^\\/lib.*"
      "^\\/usr\\/lib.*"
      "^\\/usr\\/local\\/lib.*"
      ".*system32/.*\\.dll"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(_CMAKE_TMP_dep IN LISTS _CMAKE_DEPS)
    foreach(_f IN LISTS _CMAKE_TMP_dep)
      get_filename_component(_fn "${_f}" NAME)
      list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64/${_fn}")
    endforeach()
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/../binaries/linux64" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES ${_CMAKE_TMP_dep}
      FOLLOW_SYMLINK_CHAIN)
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/lochel/OMSimulator/testsuite/resources/EventTest/sources/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
