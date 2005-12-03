# cmake project file by Prakash Punnoor
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(Alut C)
SET(PACKAGE "freealut")
SET(PACKAGE_TARNAME "freealut")
SET(PACKAGE_NAME "freealut library")
SET(PACKAGE_MAJOR_VERSION "0")
SET(PACKAGE_MINOR_VERSION "0")
SET(PACKAGE_BUILD_VERSION "0")
SET(PACKAGE_VERSION "${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}.${PACKAGE_BUILD_VERSION}")
SET(VERSION "${PACKAGE_VERSION}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
SET(PACKAGE_BUGREPORT "openal-devel@opensource.creative.com")
SET(OPERATING_SYSTEM "${CMAKE_SYSTEM_NAME}")

INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCSourceCompiles.cmake)

# config.h magic
SET(CONFIG_H_VALS_OLD "${CONFIG_H_VALS}")
SET(CONFIG_H_VALS "" CACHE INTERNAL "")
SET(CONFIG_H_FILE ${CMAKE_BINARY_DIR}/config.h)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})


MACRO(ADD_DEFINE ARG)
SET(CONFIG_H_CONTENT "${CONFIG_H_CONTENT}#define ${ARG}

")
#can't cache mulitline values
SET(CONFIG_H_VALS "${CONFIG_H_VALS}${ARG}" CACHE INTERNAL "")
ENDMACRO(ADD_DEFINE ARG)

MACRO(CHECK_INCLUDE_FILE_DEFINE HEADER VAR)
  CHECK_INCLUDE_FILE(${HEADER} HAVE_INCLUDE_FILE)
  IF(HAVE_INCLUDE_FILE)
    ADD_DEFINE("${VAR} 1")
    SET(${VAR} ON CACHE INTERNAL "")
  ENDIF(HAVE_INCLUDE_FILE)
ENDMACRO(CHECK_INCLUDE_FILE_DEFINE ARG)

MACRO(CHECK_FUNCTION_DEFINE HEADERS FUNC PARAM VAR)
CHECK_C_SOURCE_COMPILES(
"
${HEADERS}
int main(){
${FUNC} ${PARAM};
}
" ${VAR})
IF(${VAR})
  ADD_DEFINE("${VAR} 1")
ENDIF(${VAR})
ENDMACRO(CHECK_FUNCTION_DEFINE HEADERS FUNC PARAM VAR)

SET(ALUT_SOURCES src/alutBufferData.c
                 src/alutCodec.c
                 src/alutError.c
                 src/alutInit.c
                 src/alutInputStream.c
                 src/alutInternal.h
                 src/alutLoader.c
                 src/alutUtil.c
                 src/alutVersion.c
                 src/alutWaveform.c)

INCLUDE_DIRECTORIES(${Alut_SOURCE_DIR}/include)

SET(ADD_WFLAGS "")
SET(ADD_CFLAGS "")
SET(ADD_LDFLAGS "")
SET(ADD_LIBS "")

OPTION(BUILD_STATIC    "build static library too"   ON)
OPTION(WARNINGS        "enable warnings"            ON)
IF(WARNINGS)
  OPTION(MORE_WARNINGS "enable more warnings"       OFF)
  OPTION(USE_WERROR    "enable fail on all warning" OFF)
ENDIF(WARNINGS)

#want test-suit?
OPTION(BUILD_TESTS "build the test-suite" OFF)


ADD_DEFINE(ALUT_BUILD_LIBRARY)
ADD_DEFINITIONS(-D_XOPEN_SOURCE=500)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

FIND_LIBRARY(OPENAL_LIB NAMES openal openal32 PATHS /usr/lib /usr/local/lib)
IF(OPENAL_LIB MATCHES "NOTFOUND")
  MESSAGE(FATAL_ERROR "OpenAL not installed, cannot build alut - aborting.")
ENDIF(OPENAL_LIB MATCHES "NOTFOUND")

IF(UNIX)
 SET(ADD_LIBS ${ADD_LIBS} m)
ENDIF(UNIX)

CHECK_INCLUDE_FILES("AL/alc.h;AL/al.h" AL_HEADERS)
IF(NOT AL_HEADERS)
  MESSAGE(FATAL_ERROR "OpenAL header files not found - aborting.")
ENDIF(NOT AL_HEADERS)

CHECK_INCLUDE_FILE_DEFINE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_DEFINE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE_DEFINE(time.h HAVE_TIME_H)
IF(WIN32)
  CHECK_INCLUDE_FILE_DEFINE(basetsd.h HAVE_BASETSD_H)
  CHECK_INCLUDE_FILE_DEFINE(windows.h HAVE_WINDOWS_H)
ENDIF(WIN32)

# check for __attribut__
CHECK_C_SOURCE_COMPILES(
"
void  foo (int bar __attribute__((unused)) ) { }
static void baz (void) __attribute__((unused));
static void baz (void) { }
int main(){}
" HAVE___ATTRIBUTE__)

IF(HAVE___ATTRIBUTE__)
  ADD_DEFINE("HAVE___ATTRIBUTE__ 1")
ENDIF(HAVE___ATTRIBUTE__)


IF(HAVE_TIME_H)
  CHECK_FUNCTION_DEFINE("#include <time.h>" nanosleep "((struct timespec*)0, (struct timespec*)0)" HAVE_NANOSLEEP)
ENDIF(HAVE_TIME_H)

IF(HAVE_UNISTD_H)
  CHECK_FUNCTION_DEFINE("#include <unistd.h>" usleep "(0)" HAVE_USLEEP)
  CHECK_FUNCTION_DEFINE("
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  " stat "(\"\", (struct stat*)0)" HAVE_STAT)
ENDIF(HAVE_UNISTD_H)

IF(NOT HAVE_STAT)
  CHECK_FUNCTION_DEFINE("
  #include <sys/types.h>
  #include <sys/stat.h>
  " _stat "(\"\", (struct _stat*)0)" HAVE__STAT)
ENDIF(NOT HAVE_STAT)

IF(HAVE_WINDOWS_H)
  CHECK_FUNCTION_DEFINE("#include <windows.h>" Seep "(0)" HAVE_SLEEP)
ENDIF(HAVE_WINDOWS_H)


#compiler specific settings
IF(CMAKE_COMPILER_IS_GNUCC)
  IF(WARNINGS)
    SET(ADD_WFLAGS "${ADD_WFLAGS} -Wall -ansi -pedantic -W")

    IF(MORE_WARNINGS)
      SET(ADD_WFLAGS "${ADD_WFLAGS} -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Winline -Wlong-long -Wmissing-declarations -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wwrite-strings")
    ENDIF(MORE_WARNINGS)

    # Should we use turn warnings into errors?
    IF(USE_WERROR)
      SET(ADD_WFLAGS "${ADD_WFLAGS} -Werror -pedantic-errors")
    ENDIF(USE_WERROR)
  ENDIF(WARNINGS)

  IF(${CMAKE_C_FLAGS} MATCHES "^ $")
    IF(NOT ${ADD_CFLAGS} MATCHES "O")
      SET(ADD_CFLAGS "-g -O2 ${ADD_CFLAGS}")
      MESSAGE("No CFLAGS found; setting default ones.")
    ENDIF(NOT ${ADD_CFLAGS} MATCHES "O")
  ENDIF(${CMAKE_C_FLAGS} MATCHES "^ $")
ELSE(CMAKE_COMPILER_IS_GNUCC)

  MESSAGE("Your compiler isn't fully supported yet - no flags set.")
ENDIF(CMAKE_COMPILER_IS_GNUCC)


#Generate config.h only if values differ
IF(NOT CONFIG_H_VALS_OLD STREQUAL CONFIG_H_VALS OR NOT EXISTS ${CONFIG_H_FILE})
  MESSAGE(STATUS "Writing config.h")
  FILE(WRITE ${CONFIG_H_FILE} "${CONFIG_H_CONTENT}")
ENDIF(NOT CONFIG_H_VALS_OLD STREQUAL CONFIG_H_VALS OR NOT EXISTS ${CONFIG_H_FILE})

SET(CMAKE_C_FLAGS "${ADD_WFLAGS} ${ADD_CFLAGS} ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${ADD_CFLAGS} ${CMAKE_CXX_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${ADD_LDFLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${ADD_LDFLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")

IF(BUILD_STATIC)
  #we can't create a static library with the same name
  #as the shared one, so we copy it over after creation
  ADD_LIBRARY(alut-static STATIC ${ALUT_SOURCES})
  TARGET_LINK_LIBRARIES(alut-static ${OPENAL_LIB} ${ADD_LIBS})
  ADD_CUSTOM_COMMAND(
    TARGET alut-static
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy
      ${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}alut-static${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}alut${CMAKE_STATIC_LIBRARY_SUFFIX})
  INSTALL_FILES(/lib FILES ${CMAKE_STATIC_LIBRARY_PREFIX}alut${CMAKE_STATIC_LIBRARY_SUFFIX})
ENDIF(BUILD_STATIC)


ADD_LIBRARY(alut SHARED ${ALUT_SOURCES})
SET_TARGET_PROPERTIES(alut PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${PACKAGE_MAJOR_VERSION})
TARGET_LINK_LIBRARIES(alut ${OPENAL_LIB} ${ADD_LIBS})

INSTALL_TARGETS(/lib alut)
INSTALL_FILES(/include/AL FILES include/AL/alut.h)

# test-suite
IF(BUILD_TESTS)
    # examples
    ADD_EXECUTABLE(hello_world examples/hello_world.c)
    TARGET_LINK_LIBRARIES(hello_world ${OPENAL_LIB} ${ADD_LIBS} alut)

    ADD_EXECUTABLE(playfile examples/playfile.c)
    TARGET_LINK_LIBRARIES(playfile ${OPENAL_LIB} ${ADD_LIBS} alut)


    SET(TESTS errorstuff
              fileloader
              memoryloader
              version
              waveforms)

    FOREACH(TEST ${TESTS})
      SET(TEST_SRC test_suite/test_${TEST}.c)
      SET_SOURCE_FILES_PROPERTIES(${TEST_SRC} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
      ADD_EXECUTABLE(test_${TEST} ${TEST_SRC})
      TARGET_LINK_LIBRARIES(test_${TEST} ${OPENAL_LIB} ${ADD_LIBS} alut)
    ENDFOREACH(TEST)

    #copy over testdata, so test-suite can be used in binary dir
    SET(TESTDATA file1.wav
                 file2.au
                 file3.raw)

    FOREACH(TESTDATUM ${TESTDATA})
      CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/test_suite/${TESTDATUM} ${CMAKE_BINARY_DIR}/${TESTDATUM} COPYONLY)
    ENDFOREACH(TESTDATUM)
ENDIF(BUILD_TESTS)
