# Update this to have more specific rules for finding the cvode library.

find_library(CVODELibrary_LIBRARY
  NAMES sundials_cvode
  HINTS ${CVODELibrary_ROOT}/lib ${CVODELibrary_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
)

if(CVODELibrary_LIBRARY)
  get_filename_component(CVODELibrary_LIBRARYDIR ${CVODELibrary_LIBRARY} DIRECTORY)
  set(CVODELibrary_INCLUDEDIR ${CVODELibrary_ROOT}/include)
  message(STATUS "Found CVODE")
  message(STATUS "  CVODELibrary_ROOT:       " ${CVODELibrary_ROOT})
  message(STATUS "  CVODELibrary_LIBRARY:    " ${CVODELibrary_LIBRARY})
  message(STATUS "  CVODELibrary_LIBRARYDIR: " ${CVODELibrary_LIBRARYDIR})
  message(STATUS "  CVODELibrary_INCLUDEDIR: " ${CVODELibrary_INCLUDEDIR})
else()
  if(CVODELibrary_FIND_REQUIRED)
    message(STATUS "Unable to find the requested CVODELibrary" )
    message(STATUS "Looked in CVODELibrary_ROOT ${CVODELibrary_ROOT}" )
    message(SEND_ERROR "Could not find cvode. Make sure you have set the CVODELibrary_ROOT in the CMakeFile.txt to point to your CVODELibrary installation." )
  else()
    message(STATUS "CVODELibrary - NOT Found" )
  endif(CVODELibrary_FIND_REQUIRED)
endif()
