project(OMSimulator)

set(CMAKE_INSTALL_RPATH "$ORIGIN")

find_package(Threads)

include_directories(../OMSimulatorLib)
include_directories(../OMSimulatorLua)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LUALibrary_INCLUDEDIR})

link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(OMSimulator main.cpp Options.cpp)

IF(WIN32 AND MSVC)
  target_link_libraries(OMSimulator lua OMSimulatorLib fmilib sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ELSEIF (WIN32 AND MINGW)
  target_link_libraries(OMSimulator lua OMSimulatorLib fmilib shlwapi sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ELSE()
  target_link_libraries(OMSimulator lua OMSimulatorLib fmilib sundials_cvode sundials_nvecserial ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

install(TARGETS OMSimulator DESTINATION bin)
