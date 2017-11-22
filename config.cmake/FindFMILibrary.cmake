# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(FMILibrary_LIBRARY
  NAMES fmilib fmilib_shared
  HINTS ${FMILibrary_ROOT}/lib
)

if(FMILibrary_LIBRARY)
  set(FMILibrary_LIBRARYDIR ${FMILibrary_ROOT}/lib)
  set(FMILibrary_INCLUDEDIR ${FMILibrary_ROOT}/include)
  message(STATUS "Found FMILibrary")
  message(STATUS "  FMILibrary_ROOT:       " ${FMILibrary_ROOT})
  message(STATUS "  FMILibrary_LIBRARY:    " ${FMILibrary_LIBRARY})
  message(STATUS "  FMILibrary_LIBRARYDIR: " ${FMILibrary_LIBRARYDIR})
  message(STATUS "  FMILibrary_INCLUDEDIR: " ${FMILibrary_INCLUDEDIR})
else()
  if(FMILibrary_FIND_REQUIRED)
    message(STATUS "Unable to find the requested FMILibrary" )
    message(STATUS "Looked in FMILibrary_ROOT ${FMILibrary_ROOT}" )
    message(SEND_ERROR "Could not find fmilib. Make sure you have set the FMILibrary_ROOT in the CMakeFile.txt to point to your FMILibrary installation." )
  else()
    message(STATUS "FMILibrary - NOT Found" )
  endif(FMILibrary_FIND_REQUIRED)
endif()
