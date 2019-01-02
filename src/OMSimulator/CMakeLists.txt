project(OMSimulator)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/${HOST_SHORT}:$ORIGIN/")

include_directories(../OMSimulatorLib)
include_directories(../OMSysIdentLib)
include_directories(../OMSimulatorLua)
include_directories(${LUALibrary_INCLUDEDIR})

link_directories(${FMILibrary_LIBRARYDIR})
link_directories(${LUALibrary_LIBRARYDIR})
link_directories(${CVODELibrary_LIBRARYDIR})
link_directories(${KINSOLLibrary_LIBRARYDIR})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${MINIZIP_LIBRARYDIR})
IF (OMTLM)
  link_directories(${OMTLMSIMULATORLIB_LIBRARYDIR})
ENDIF ()

add_executable(OMSimulator main.cpp)

IF (Ceres_FOUND AND OMSYSIDENT)
  add_definitions(-DWITH_OMSYSIDENT)
ENDIF()

IF (WIN32 AND MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF ()

add_definitions(-DOMS_STATIC)
target_link_libraries(OMSimulator OMSimulatorLib_static lua)

install(TARGETS OMSimulator DESTINATION bin)
