# Update this to have more specific rules for finding the kinsol library.

find_library(KINSOLLibrary_LIBRARY
  NAMES sundials_kinsol
  HINTS ${KINSOLLibrary_ROOT}/lib ${KINSOLLibrary_ROOT}/lib/x86_64-linux-gnu ${KINSOLLibrary_ROOT}/lib/i386-linux-gnu
)

if(KINSOLLibrary_LIBRARY)
  set(KINSOLLibrary_LIBRARYDIR ${KINSOLLibrary_ROOT}/lib)
  set(KINSOLLibrary_INCLUDEDIR ${KINSOLLibrary_ROOT}/include)
  message(STATUS "Found KINSOL")
  message(STATUS "  KINSOLLibrary_ROOT:       " ${KINSOLLibrary_ROOT})
  message(STATUS "  KINSOLLibrary_LIBRARY:    " ${KINSOLLibrary_LIBRARY})
  message(STATUS "  KINSOLLibrary_LIBRARYDIR: " ${KINSOLLibrary_LIBRARYDIR})
  message(STATUS "  KINSOLLibrary_INCLUDEDIR: " ${KINSOLLibrary_INCLUDEDIR})
else()
  if(KINSOLLibrary_FIND_REQUIRED)
    message(STATUS "Unable to find the requested KINSOLLibrary" )
    message(STATUS "Looked in KINSOLLibrary_ROOT ${KINSOLLibrary_ROOT}" )
    message(SEND_ERROR "Could not find kinsol. Make sure you have set the KINSOLLibrary_ROOT in the CMakeFile.txt to point to your KINSOLLibrary installation." )
  else()
    message(STATUS "KINSOLLibrary - NOT Found" )
  endif(KINSOLLibrary_FIND_REQUIRED)
endif()
