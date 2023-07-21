# Update this to have more specific rules for finding the fmi library.
# e.g versions, shared or static ...
# for now this should suffice.
# maybe make sure the headers exits too.

find_library(LUALibrary_LIBRARY
  NAMES lua_static
  HINTS ${LUALibrary_ROOT}/lib ${LUALibrary_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
)

if(LUALibrary_LIBRARY)
  get_filename_component(LUALibrary_LIBRARYDIR ${LUALibrary_LIBRARY} DIRECTORY)
  set(LUALibrary_INCLUDEDIR ${LUALibrary_ROOT}/include)
  message(STATUS "Found LUA")
  message(STATUS "  LUALibrary_ROOT:       " ${LUALibrary_ROOT})
  message(STATUS "  LUALibrary_LIBRARY:    " ${LUALibrary_LIBRARY})
  message(STATUS "  LUALibrary_LIBRARYDIR: " ${LUALibrary_LIBRARYDIR})
  message(STATUS "  LUALibrary_INCLUDEDIR: " ${LUALibrary_INCLUDEDIR})
else()
  if(LUALibrary_FIND_REQUIRED)
    message(STATUS "Unable to find the requested LUALibrary" )
    message(STATUS "Looked in LUALibrary_ROOT ${LUALibrary_ROOT}" )
    message(SEND_ERROR "Could not find lua. Make sure you have set the LUALibrary_ROOT in the CMakeFile.txt to point to your LUALibrary installation." )
  else()
    message(STATUS "LUALibrary - NOT Found" )
  endif(LUALibrary_FIND_REQUIRED)
endif()
