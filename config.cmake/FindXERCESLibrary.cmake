# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(XERCESLibrary_LIBRARY
  NAMES xerces-c
  HINTS ${XERCESLibrary_ROOT} ${XERCESLibrary_ROOT}/lib
)

if (XERCESLibrary_LIBRARY)
  message(STATUS "Found XERCES")
  set(XERCESLibrary_INCLUDEDIR ${XERCESLibrary_ROOT}/include)
  set(XERCESLibrary_LIBRARYDIR ${XERCESLibrary_ROOT}/lib)
  message(STATUS "Xerces_ROOT:                " ${XERCESLibrary_ROOT})
  message(STATUS "XercesLibrary_LIBRARY:      " ${XERCESLibrary_LIBRARY})
  message(STATUS "XercesC_INCLUDE_DIRS:       " ${XERCESLibrary_INCLUDEDIR})
  message(STATUS "XercesC_LIBRARIES:          " ${XERCESLibrary_LIBRARYDIR})
else()
  if(XERCESLibrary_FIND_REQUIRED)
    message(STATUS "Unable to find the requested Xerces Library" )
    message(STATUS "Looked in XERCESLibrary_ROOT ${XERCESLibrary_ROOT}" )
    message(SEND_ERROR "Could not find xerces. Make sure you have set the XERCESLibrary_ROOT in the CMakeFile.txt to point to your XERCESLibrary installation." )
  else()
    message(STATUS "XERCESLibrary - NOT Found" )
  endif(XERCESLibrary_FIND_REQUIRED)
endif()

