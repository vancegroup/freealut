# cmake project file by Prakash Punnoor
cmake_minimum_required(VERSION 2.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/admin/CMakeModules")
project(Alut C)
set(PACKAGE "freealut")
set(PACKAGE_TARNAME "freealut")
set(PACKAGE_NAME "freealut library")
set(PACKAGE_MAJOR_VERSION "1")
set(PACKAGE_MINOR_VERSION "0")
set(PACKAGE_BUILD_VERSION "1")
set(PACKAGE_VERSION
	"${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}.${PACKAGE_BUILD_VERSION}")
set(MAJOR_VERSION "0")
set(MINOR_VERSION "0")
set(BUILD_VERSION "0")
set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "openal-devel@opensource.creative.com")
set(OPERATING_SYSTEM "${CMAKE_SYSTEM_NAME}")

include(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
include(${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
include(${CMAKE_ROOT}/Modules/CheckCSourceCompiles.cmake)

find_package(ConfigHelper)

set(ALUT_SOURCES
	src/alutBufferData.c
	src/alutCodec.c
	src/alutError.c
	src/alutInit.c
	src/alutInputStream.c
	src/alutInternal.h
	src/alutLoader.c
	src/alutOutputStream.c
	src/alutUtil.c
	src/alutVersion.c
	src/alutWaveform.c)

include_directories(${Alut_SOURCE_DIR}/include)

set(ADD_WFLAGS "")
set(ADD_CFLAGS "")
set(ADD_LDFLAGS "")
set(ADD_LIBS "")

option(BUILD_STATIC "build static library too" OFF)
option(PROFILE "enable profile" OFF)
option(OPTIMIZATION "enable optimization" ON)
option(WARNINGS "enable warnings" OFF)
if(WARNINGS)
	option(MORE_WARNINGS "enable more warnings" OFF)
	option(USE_WERROR "enable fail on all warning" OFF)
endif()

#want test-suit?
option(BUILD_TESTS "build the test-suite" OFF)


add_define(ALUT_BUILD_LIBRARY)
# We could possibly need struct timespec and random(), which are not ANSI.
# Define to 500 if Single Unix conformance is wanted, 600 for sixth revision.
add_define("_XOPEN_SOURCE 600")
# We might need nanosleep, which is a POSIX IEEE Std 1003.1b-1993 feature.
# Define to the POSIX version that should be used.
add_define("_POSIX_C_SOURCE 200112L")
# Without __NO_CTYPE tolower and friends are macros which introduce a GLIBC 2.3
# dependency. By defining this identifier we are currently backwards compatible
# to GLIBC 2.1.3, which is a good thing. In addition, the macros lead to code
# which triggers warnings with -Wunreachable-code.
add_define("__NO_CTYPE 1")

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DNDEBUG)

find_library(OPENAL_LIB
	NAMES
	openal
	openal32
	PATHS
	/usr/lib
	/usr/local/lib
	${OPENAL_LIB_DIR})
if(OPENAL_LIB MATCHES "NOTFOUND")
	message(FATAL_ERROR
		"OpenAL not installed, cannot build alut - aborting.")
endif()

if(UNIX)
	set(ADD_LIBS ${ADD_LIBS} m)
endif()

set(CMAKE_REQUIRED_INCLUDES ${OPENAL_INCLUDE_DIR})
check_include_files("AL/alc.h;AL/al.h" AL_HEADERS)
if(NOT AL_HEADERS)
	message(FATAL_ERROR "OpenAL header files not found - aborting.")
endif()

if(DEFINED OPENAL_INCLUDE_DIR)
	include_directories(${OPENAL_INCLUDE_DIR})
endif()

# checks for unistd.h and time.h, as well
find_package(SleepFunction)

check_include_file_define(stdint.h HAVE_STDINT_H)

if(WIN32)
	# check for __int8 type
	set(SOURCE "int main(){__int8 foo;return 0;}
")
	file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/src.c" "${SOURCE}")

	try_compile(HAVE___INT8
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/CMakeTmp/src.c
		CMAKE_FLAGS
		OUTPUT_VARIABLE
		OUTPUT)
	if(HAVE___INT8)
		add_define("HAVE___INT8 1")
	endif()
endif()

if(HAVE_UNISTD_H)
	check_function_define("
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  "
		stat
		"(\"\", (struct stat*)0)"
		HAVE_STAT)
endif()

if(NOT HAVE_STAT)
	check_function_define("
  #include <sys/types.h>
  #include <sys/stat.h>
  "
		_stat
		"(\"\", (struct _stat*)0)"
		HAVE__STAT)
endif()

# compiler specific settings
find_package(CompilerAttribute)
find_package(CompilerVisibility)
find_package(CompilerFlagsSet)

generate_config_h()

set(CMAKE_C_FLAGS "${ADD_WFLAGS} ${ADD_CFLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${ADD_CFLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS
	"${ADD_LDFLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS
	"${ADD_LDFLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")

if(BUILD_STATIC)
	# we can't create a static library with the same name
	# as the shared one, so we copy it over after creation
	add_library(alut_static STATIC ${ALUT_SOURCES})
	target_link_libraries(alut_static ${OPENAL_LIB} ${ADD_LIBS})
	if(NOT WIN32)
		add_custom_command(TARGET
			alut_static
			POST_BUILD
			COMMAND
			${CMAKE_COMMAND}
			ARGS
			-E
			copy
			${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}alut_static${CMAKE_STATIC_LIBRARY_SUFFIX}
			${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}alut${CMAKE_STATIC_LIBRARY_SUFFIX})
		install_files(/lib
			FILES
			${CMAKE_STATIC_LIBRARY_PREFIX}alut${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
endif()


add_library(alut SHARED ${ALUT_SOURCES})
set_target_properties(alut
	PROPERTIES
	VERSION
	${VERSION}
	SOVERSION
	${MAJOR_VERSION})
target_link_libraries(alut ${OPENAL_LIB} ${ADD_LIBS})

install_targets(/lib alut)
install_files(/include/AL FILES include/AL/alut.h)

# needed for openal.pc.in and openal-config.in
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(bindir "\${exec_prefix}/bin")
set(includedir "\${prefix}/include")
set(requirements "")
# static linking dependecies are broken, so pthread isn't needed currently
set(PTHREAD_LIBS ${CMAKE_THREAD_LIBS_INIT})

configure_file(${CMAKE_SOURCE_DIR}/admin/pkgconfig/freealut.pc.in
	${CMAKE_BINARY_DIR}/admin/pkgconfig/freealut.pc
	@ONLY)
configure_file(${CMAKE_SOURCE_DIR}/admin/pkgconfig/freealut-config.in
	${CMAKE_BINARY_DIR}/admin/pkgconfig/freealut-config
	@ONLY)
install_programs(/bin FILES admin/pkgconfig/freealut-config)
install_files(/lib/pkgconfig FILES admin/pkgconfig/freealut.pc)


# test-suite
if(BUILD_TESTS)
	# examples
	add_executable(hello_world examples/hello_world.c)
	target_link_libraries(hello_world ${OPENAL_LIB} ${ADD_LIBS} alut)

	add_executable(playfile examples/playfile.c)
	target_link_libraries(playfile ${OPENAL_LIB} ${ADD_LIBS} alut)


	set(TESTS errorstuff fileloader memoryloader version waveforms)

	foreach(TEST ${TESTS})
		set(TEST_SRC test_suite/test_${TEST}.c)
		if(CMAKE_COMPILER_IS_GNUCC)
			set_source_files_properties(${TEST_SRC}
				PROPERTIES
				COMPILE_FLAGS
				"-Wno-deprecated-declarations")
		endif()
		add_executable(test_${TEST} ${TEST_SRC})
		target_link_libraries(test_${TEST} ${OPENAL_LIB} ${ADD_LIBS} alut)
	endforeach()

	#copy over testdata, so test-suite can be used in binary dir
	set(TESTDATA file1.wav file2.au file3.raw)

	foreach(TESTDATUM ${TESTDATA})
		configure_file(${CMAKE_SOURCE_DIR}/test_suite/${TESTDATUM}
			${CMAKE_BINARY_DIR}/${TESTDATUM}
			COPYONLY)
	endforeach()
endif()
