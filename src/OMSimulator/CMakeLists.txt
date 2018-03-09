project(OMSimulator)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../bin:$ORIGIN/")

find_package(Threads)

# Because the shared libraries cannot link to static boost on amd64,
# we have a different set of options for finding the boost package
# used for the executable itself.

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost 1.54.0 COMPONENTS system filesystem REQUIRED)
IF(Boost_FOUND)
  message(STATUS "Found Boost static libraries")
  message(STATUS "  Boost_LIBRARIES:    " ${Boost_LIBRARIES})
  message(STATUS "  Boost_LIBRARY_DIRS: " ${Boost_LIBRARY_DIRS})
  message(STATUS "  Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS})
ELSE()
  MESSAGE(WARNING, "Boost library not found, please give a hint by setting the cmake variable BOOST_ROOT either in the cmake-gui or the command line, e.g., 'cmake -DBOOST_ROOT=C:/local/boost_1_63_0'")
ENDIF()

include_directories(../OMSimulatorLib)
include_directories(../OMSimulatorLua)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LUALibrary_INCLUDEDIR})
include_directories(../OMFitLib)
include_directories(../OMFitLua)


link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(OMSimulator main.cpp Options.cpp)

IF (Ceres_FOUND AND OMFIT)
  set(OMFIT_OPTION OMFit)
  set(CERES_OPTION ${CERES_LIBRARIES})
  add_definitions(-DWITH_OMFIT)
ENDIF()

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulator lua ${OMFIT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulator lua ${OMFIT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} shlwapi sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ELSE()
  target_link_libraries(OMSimulator lua ${OMFIT_OPTION} OMSimulatorLib ${FMILibrary_LIBRARY} sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CERES_OPTION} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

IF ((NOT APPLE) AND CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang|GNU")
  # Avoid external dependencies; especially useful when cross-compiling
  # embedded ARM Linux systems
  message(STATUS "Link the executable using static libgcc and libstdc++")
  target_link_libraries(OMSimulator -static-libgcc -static-libstdc++)
ENDIF()

install(TARGETS OMSimulator DESTINATION bin)
