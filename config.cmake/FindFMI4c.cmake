# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(FMI4c_LIBRARY
  NAMES fmi4c fmi4c_shared
  HINTS ${FMI4c_ROOT}/lib
)

if(FMI4c_LIBRARY)
  set(FMI4c_LIBRARYDIR ${FMI4c_ROOT}/lib)
  set(FMI4c_INCLUDEDIR ${FMI4c_ROOT}/include)
  message(STATUS "Found FMI4c")
  message(STATUS "  FMI4c_ROOT:       " ${FMI4c_ROOT})
  message(STATUS "  FMI4c_LIBRARY:    " ${FMI4c_LIBRARY})
  message(STATUS "  FMI4c_LIBRARYDIR: " ${FMI4c_LIBRARYDIR})
  message(STATUS "  FMI4c_INCLUDEDIR: " ${FMI4c_INCLUDEDIR})
else()
  if(FMI4c_FIND_REQUIRED)
    message(STATUS "Unable to find the requested FMI4c" )
    message(STATUS "Looked in FMI4c_ROOT ${FMI4c_ROOT}" )
    message(SEND_ERROR "Could not find libfmi4c. Make sure you have set the FMI4c_ROOT in the CMakeFile.txt to point to your FMI4c installation." )
  else()
    message(STATUS "FMI4c - NOT Found" )
  endif(FMI4c_FIND_REQUIRED)
endif()
