SET(CMAKE_SYSTEM_NAME Linux)  # Tell CMake we're cross-compiling

include(CMakeForceCompiler)
# Prefix detection only works with compiler id "GNU"
# CMake will look for prefixed g++, cpp, ld, etc. automatically
CMAKE_FORCE_C_COMPILER(arm-linux-androideabi-gcc GNU)

set(CMAKE_C_FLAGS "-fpie -fPIE" CACHE STRING "" FORCE)
set(CMAKE_LD_FLAGS "-pie" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "-pie" CACHE STRING "" FORCE)

SET(ANDROID TRUE)

# static library
SET(LIBTYPE STATIC)
