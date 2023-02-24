# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(ZLIB_LIBRARY
  NAMES zlibstatic
  HINTS ${ZLIB_ROOT}/lib
  NO_DEFAULT_PATH
)

if(ZLIB_LIBRARY)
  set(ZLIB_LIBRARYDIR ${ZLIB_ROOT}/lib)
  set(ZLIB_INCLUDEDIR ${ZLIB_ROOT}/include)
  message(STATUS "Found ZLIB")
  message(STATUS "  ZLIB_ROOT:       " ${ZLIB_ROOT})
  message(STATUS "  ZLIB_LIBRARY:    " ${ZLIB_LIBRARY})
  message(STATUS "  ZLIB_LIBRARYDIR: " ${ZLIB_LIBRARYDIR})
  message(STATUS "  ZLIB_INCLUDEDIR: " ${ZLIB_INCLUDEDIR})
else()
  if(ZLIB_FIND_REQUIRED)
    message(STATUS "Unable to find the requested ZLIB" )
    message(STATUS "Looked in ZLIB_ROOT ${ZLIB_ROOT}" )
    message(SEND_ERROR "Could not find libZLIB. Make sure you have set the ZLIB_ROOT in the CMakeFile.txt to point to your ZLIB installation." )
  else()
    message(STATUS "ZLIB - NOT Found" )
  endif(ZLIB_FIND_REQUIRED)
endif()
