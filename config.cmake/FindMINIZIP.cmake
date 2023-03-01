# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(MINIZIP_LIBRARY
  NAMES minizip
  HINTS ${MINIZIP_ROOT}/lib
  NO_DEFAULT_PATH
)

if(MINIZIP_LIBRARY)
  set(MINIZIP_LIBRARYDIR ${MINIZIP_ROOT}/lib)
  set(MINIZIP_INCLUDEDIR ${MINIZIP_ROOT}/include)
  message(STATUS "Found MINIZIP")
  message(STATUS "  MINIZIP_ROOT:       " ${MINIZIP_ROOT})
  message(STATUS "  MINIZIP_LIBRARY:    " ${MINIZIP_LIBRARY})
  message(STATUS "  MINIZIP_LIBRARYDIR: " ${MINIZIP_LIBRARYDIR})
  message(STATUS "  MINIZIP_INCLUDEDIR: " ${MINIZIP_INCLUDEDIR})
else()
  if(MINIZIP_FIND_REQUIRED)
    message(STATUS "Unable to find the requested MINIZIP" )
    message(STATUS "Looked in MINIZIP_ROOT ${MINIZIP_ROOT}" )
    message(SEND_ERROR "Could not find libminizip.a Make sure you have set the MINIZIP_ROOT in the CMakeFile.txt to point to your MINIZIP installation." )
  else()
    message(STATUS "MINIZIP - NOT Found" )
  endif(MINIZIP_FIND_REQUIRED)
endif()
