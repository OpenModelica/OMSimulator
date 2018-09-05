project(OMSimulator)

IF(WIN32)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../bin:$ORIGIN/")
ELSE(WIN32)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")
ENDIF(WIN32)
find_package(Threads)

# Because the shared libraries cannot link to static boost on amd64,
# we have a different set of options for finding the boost package
# used for the executable itself.

set(Boost_USE_STATIC_LIBS ON)
IF(NOT WIN32)
set(Boost_USE_STATIC_RUNTIME ON)
ENDIF()

find_package(Boost 1.41.0 COMPONENTS filesystem system ${LIB_BOOST_REGEX} ${LIB_BOOST_ATOMIC} REQUIRED)
IF(Boost_FOUND)
  message(STATUS "Found Boost static libraries")
  message(STATUS "  Boost_LIBRARIES:    " ${Boost_LIBRARIES})
  message(STATUS "  Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
  message(STATUS "  Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS})
ELSE()
  MESSAGE(WARNING, "Boost library not found, please give a hint by setting the cmake variable BOOST_ROOT either in the cmake-gui or the command line, e.g., 'cmake -DBOOST_ROOT=C:/local/boost_1_63_0'")
ENDIF()

include_directories(../../3rdParty/RegEx)
include_directories(../OMSimulatorLib)
include_directories(../OMSimulatorLua)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LUALibrary_INCLUDEDIR})
include_directories(../OMSysIdentLib)
include_directories(../OMSysIdentLua)

link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${Boost_LIBRARY_DIRS})
IF(OMTLM)
link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF()

add_executable(OMSimulator main.cpp Options.cpp)

IF (Ceres_FOUND AND OMSYSIDENT)
  set(OMSYSIDENT_OPTION OMSysIdent)
  set(CERES_OPTION ${CERES_LIBRARIES})
  add_definitions(-DWITH_OMSYSIDENT)
ENDIF()

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulator lua ${OMSYSIDENT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OMTLM_LINKFLAGS})
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulator lua ${OMSYSIDENT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} shlwapi sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OMTLM_LINKFLAGS} ${LIB_ATOMIC})
ELSE()
  target_link_libraries(OMSimulator lua ${OMSYSIDENT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OMTLM_LINKFLAGS} ${LIB_ATOMIC})
ENDIF()

IF ((NOT APPLE) AND CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang|GNU")
  # Avoid external dependencies; especially useful when cross-compiling
  # embedded ARM Linux systems
  message(STATUS "Link the executable using static libgcc and libstdc++")
IF (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
  target_link_libraries(OMSimulator -static-libgcc -lstdc++)
ELSE()
  target_link_libraries(OMSimulator -static-libgcc -static-libstdc++)
ENDIF()

ENDIF()

install(TARGETS OMSimulator DESTINATION bin)
