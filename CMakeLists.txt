# cmake project file by Prakash Punnoor
# improved by Ryan Pavlik
cmake_minimum_required(VERSION 2.6)

project(Alut C)

set(PACKAGE "freealut")
set(PACKAGE_TARNAME "freealut")
set(PACKAGE_NAME "freealut library")
set(PACKAGE_MAJOR_VERSION "1")
set(PACKAGE_MINOR_VERSION "1")
set(PACKAGE_BUILD_VERSION "0")
set(PACKAGE_VERSION
	"${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}.${PACKAGE_BUILD_VERSION}")
set(MAJOR_VERSION "0")
set(MINOR_VERSION "0")
set(BUILD_VERSION "0")
set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "openal-devel@opensource.creative.com")
set(OPERATING_SYSTEM "${CMAKE_SYSTEM_NAME}")

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


# We could possibly need struct timespec and random(), which are not ANSI.
# Define to 500 if Single Unix conformance is wanted, 600 for sixth revision.
#add_definitions(-D_XOPEN_SOURCE=600)
# We might need nanosleep, which is a POSIX IEEE Std 1003.1b-1993 feature.
# Define to the POSIX version that should be used.
#add_definitions(-D_POSIX_C_SOURCE=200112L)
# Without __NO_CTYPE tolower and friends are macros which introduce a GLIBC 2.3
# dependency. By defining this identifier we are currently backwards compatible
# to GLIBC 2.1.3, which is a good thing. In addition, the macros lead to code
# which triggers warnings with -Wunreachable-code.
#add_definitions(-D__NO_CTYPE=1)

add_definitions(-DHAVE_CONFIG_H)

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})


if(UNIX)
	set(ADD_LIBS ${ADD_LIBS} m)
endif()

###
# Checking for types
###
if(WIN32)
	include(CheckTypeSize)
	check_type_size(__int8 HAVE___INT8)
endif()

###
# Checking for Includes
###
include(CheckIncludeFile)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(windows.h HAVE_WINDOWS_H)

###
# Checking for functions
###
include(CheckSymbolExists)

check_symbol_exists(_stat "sys/types.h;sys/stat.h" HAVE__STAT)
if(HAVE_TIME_H)
	check_symbol_exists(nanosleep "time.h" HAVE_NANOSLEEP)
endif()
if(HAVE_UNISTD_H)
	check_symbol_exists(usleep "unistd.h" HAVE_USLEEP)
	check_symbol_exists(stat "sys/types.h;sys/stat.h;unistd.h" HAVE_STAT)
endif()
if(HAVE_WINDOWS_H)
	check_symbol_exists(Sleep "windows.h" HAVE_SLEEP)
endif()

###
# Checking for __attribute__
###
include(CheckCSourceCompiles)
check_c_source_compiles("void  foo (int bar __attribute__((unused)) ) { }
	static void baz (void) __attribute__((unused));
	static void baz (void) { }
	int main(){}
	"
	HAVE___ATTRIBUTE__)

###
# Checking for __attribute__((visibility("default"))) and -fvisibility=hidden
###
set(CMAKE_REQUIRED_FLAGS "-fvisibility=hidden")
check_c_source_compiles("void __attribute__((visibility(\"default\"))) test() {}
	#ifdef __INTEL_COMPILER
	#error ICC breaks with binutils and visibility
	#endif
	int main(){}
	" HAVE_GCC_VISIBILITY)
set(CMAKE_REQUIRED_FLAGS)

if(HAVE_GCC_VISIBILITY)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	add_definitions(-DHAVE_GCC_VISIBILITY)
endif()

###
# Checking compiler flags
###
include(CheckCCompilerFlag)
set(NEW_FLAGS)
set(POTENTIAL_FLAGS)
if(NOT MSVC)
	if(OPTIMIZATION)
		list(APPEND POTENTIAL_FLAGS -finline-functions -ffast-math)
	endif()
	if(PROFILE)
		list(APPEND POTENTIAL_FLAGS -pg)
	elseif(OPTIMIZATION)
		# -pg and -fomit-frame-pointer are incompatible
		list(APPEND POTENTIAL_FLAGS -fomit-frame-pointer)
	endif()
	if(WARNINGS)
		list(APPEND POTENTIAL_FLAGS -Wall -ansi -pedantic -W)
		if(MORE_WARNINGS)
			list(APPEND POTENTIAL_FLAGS -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels -Winline -Wlong-long -Wmissing-declarations -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wwrite-strings)
		endif()
	endif()
endif()

if(POTENTIAL_FLAGS)
	foreach(FLAG ${POTENTIAL_FLAGS})
		string(REGEX REPLACE "^-" "COMPILER_SUPPORTS_" VAR "${FLAG}")
		check_c_compiler_flag("${FLAG}" ${VAR})
		if(${${VAR}})
			list(APPEND NEW_FLAGS ${FLAG})
		endif()
	endforeach()
	if(NEW_FLAGS)
		string(REPLACE ";" " " NEW_FLAGS "${NEW_FLAGS}")
		message(STATUS "Building with additional flags: ${NEW_FLAGS}")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NEW_FLAGS}")
	endif()
endif()

###
# Generate the config header
###
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake_in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

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
