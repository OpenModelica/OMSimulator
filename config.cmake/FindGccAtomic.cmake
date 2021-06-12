# This scripts finds gcc's built-in atomic shared library (libatomic.so).

# When/Why you need this.
# There are a few libraries with similar names (eg libatomic-ops), so do not be confused.
# It's the shared library that is a part of newer gcc, and required to execute gcc methods like:
# "__atomic_compare_exchange_16". You can use the 16-byte cas without linking to libatomic
# on x86 (as far as you give -mcx16), but on AArch64 you have to link to it and gcc doesn't allow
# -mcx16.

# In Fedora 19-x86, you typically find it at: /lib64/libatomic.so.1
# In Ubuntu-AArch64, you typically find it at: /usr/lib/aarch64-linux-gnu/libatomic.so.1

# In Fedora 19-x86, you have to separately install it (yum install libatomic).
# In Ubuntu-AArch64, seems like it's automatically installed with gcc. (To check, apt-get install libatomic1).


# Note that this is a shared library. Every atomic operation now causes some overhead to
# go through another module, but hopefully it's negligible.

# Output variables (as the methods are gcc-builtin, no includes):
#  GCCLIBATOMIC_LIBRARY   : Library path of libatomic.so
#  GCCLIBATOMIC_FOUND     : True if found.

# I'm also putting "atomic.so.1" because at least FC19 and Ubuntu's repo don't create
# "libatomic.so" symlink. They only have libatomic.so.1.0.0 and libatomic.so.1 symlink. No idea why.
FIND_LIBRARY(GCCLIBATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic.so.1
  HINTS
  $ENV{HOME}/local/lib64
  $ENV{HOME}/local/lib
  /usr/local/lib64
  /usr/local/lib
  /opt/local/lib64
  /opt/local/lib
  /usr/lib64
  /usr/lib
  /lib64
  /lib
)

IF (GCCLIBATOMIC_LIBRARY)
    SET(GCCLIBATOMIC_FOUND TRUE)
    MESSAGE(STATUS "Found GCC's libatomic.so: lib=${GCCLIBATOMIC_LIBRARY}")
ELSE ()
    SET(GCCLIBATOMIC_FOUND FALSE)
    MESSAGE(STATUS "GCC's libatomic.so not found. This library is essential in AArch64 (if you are on x86, this doesn't matter)")
    MESSAGE(STATUS "Try: 'sudo yum install libatomic' (or sudo apt-get install libatomic1)")
ENDIF ()
