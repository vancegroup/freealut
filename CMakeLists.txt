# cmake project file by Prakash Punnoor
# improved by Rylie Pavlik
cmake_minimum_required(VERSION 3.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(Alut C)

include(GNUInstallDirs)

set(PACKAGE "freealut")
set(PACKAGE_TARNAME "freealut")
set(PACKAGE_NAME "freealut library")
set(PACKAGE_MAJOR_VERSION "1")
set(PACKAGE_MINOR_VERSION "1")
set(PACKAGE_BUILD_VERSION "0")
set(PACKAGE_VERSION
	"${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}.${PACKAGE_BUILD_VERSION}"
)

# For SONAME
set(MAJOR_VERSION "0")
set(MINOR_VERSION "1")
set(BUILD_VERSION "0")

set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "openal-devel@opensource.creative.com")
set(OPERATING_SYSTEM "${CMAKE_SYSTEM_NAME}")

# What to build?
option(BUILD_EXAMPLES "build example applications" ON)
option(BUILD_SHARED_LIBS
	   "build freealut as a shared library rather than a static library" ON)
option(BUILD_TESTS "build the test-suite" ON)

# How to build it?
option(BUILD_PROFILE "enable profile" OFF)
option(BUILD_OPTIMIZATION "enable optimization" ON)
option(BUILD_WARNINGS "enable warnings" OFF)
option(BUILD_MORE_WARNINGS "enable more warnings" OFF)
option(BUILD_USE_WERROR "enable fail on all warning" OFF)

# Dependencies
find_package(OpenAL REQUIRED)

# Checking for types
if(WIN32)
	include(CheckTypeSize)
	check_type_size(__int8 HAVE___INT8)
endif()

# Checking for Includes
include(CheckIncludeFile)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_include_file(features.h HAVE_FEATURES_H)

# Checking for functions
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

# Checking for GLIBC
if(HAVE_FEATURES_H)
	check_symbol_exists(__GLIBC__ "features.h" HAVE_GLIBC)
endif()

# Checking for __attribute__
include(CheckCSourceCompiles)
check_c_source_compiles(
	"void  foo (int bar __attribute__((unused)) ) { }
	static void baz (void) __attribute__((unused));
	static void baz (void) { }
	int main(){}
	"
	HAVE___ATTRIBUTE__)

# Checking for __attribute__((visibility("default"))) and -fvisibility=hidden
set(CMAKE_REQUIRED_FLAGS "-fvisibility=hidden")
check_c_source_compiles(
	"void __attribute__((visibility(\"default\"))) test() {}
	#ifdef __INTEL_COMPILER
	#error ICC breaks with binutils and visibility
	#endif
	int main(){}
	"
	HAVE_GCC_VISIBILITY)
set(CMAKE_REQUIRED_FLAGS)

# Checking compiler flags
include(CheckCCompilerFlag)
set(NEW_FLAGS)
set(POTENTIAL_FLAGS)
if(NOT MSVC)
	if(BUILD_OPTIMIZATION)
		list(APPEND POTENTIAL_FLAGS -finline-functions -ffast-math)
	endif()

	if(BUILD_PROFILE)
		list(APPEND POTENTIAL_FLAGS -pg)
	elseif(BUILD_OPTIMIZATION)
		# -pg and -fomit-frame-pointer are incompatible
		list(APPEND POTENTIAL_FLAGS -fomit-frame-pointer)
	endif()

	if(BUILD_WARNINGS)
		list(APPEND POTENTIAL_FLAGS -Wall -ansi -pedantic -W)

		if(BUILD_MORE_WARNINGS)
			list(
				APPEND
				POTENTIAL_FLAGS
				-Waggregate-return
				-Wbad-function-cast
				-Wcast-align
				-Wcast-qual
				-Wdisabled-optimization
				-Wendif-labels
				-Winline
				-Wlong-long
				-Wmissing-declarations
				-Wmissing-noreturn
				-Wmissing-prototypes
				-Wnested-externs
				-Wpacked
				-Wpointer-arith
				-Wredundant-decls
				-Wshadow
				-Wsign-compare
				-Wstrict-prototypes
				-Wwrite-strings)
		endif()

		if(BUILD_USE_WERROR)
			list(APPEND POTENTIAL_FLAGS -Werror)
		endif()
	else()

		if(BUILD_MORE_WARNINGS)
			message(
				STATUS
					"NOTE: BUILD_MORE_WARNINGS ignored because BUILD_WARNINGS not set!"
			)
		endif()

		if(BUILD_USE_WERROR)
			message(
				STATUS
					"NOTE: BUILD_USE_WERROR ignored because BUILD_WARNINGS not set!"
			)
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

# Generate the config header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake_in"
			   "${CMAKE_CURRENT_BINARY_DIR}/config.h")

# Build the library
add_subdirectory(src)

# Make the pkg-config and config files
#
# needed for openal.pc.in and openal-config.in
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
set(bindir "${CMAKE_INSTALL_FULL_BINDIR}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(extra_cflags "")
if(BUILD_SHARED_LIBS)
	set(extra_cflags "-DALUT_STATIC")
endif()
set(requirements "")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/admin/pkgconfig/freealut.pc.in"
			   "${CMAKE_CURRENT_BINARY_DIR}/admin/pkgconfig/freealut.pc" @ONLY)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/admin/pkgconfig/freealut-config.in"
	"${CMAKE_CURRENT_BINARY_DIR}/admin/pkgconfig/freealut-config" @ONLY)
install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/admin/pkgconfig/freealut-config"
		DESTINATION bin)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/admin/pkgconfig/freealut.pc"
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Examples
add_subdirectory(examples)

# test-suite
if(BUILD_TESTS)
	include(CTest)

	set(TESTS errorstuff fileloader memoryloader version waveforms)

	foreach(TEST ${TESTS})
		set(TEST_SRC test_suite/test_${TEST}.c)
		if(CMAKE_COMPILER_IS_GNUCC)
			set_source_files_properties(
				${TEST_SRC} PROPERTIES COMPILE_FLAGS
									   "-Wno-deprecated-declarations")
		endif()
		add_executable(test_${TEST} ${TEST_SRC})
		target_link_libraries(test_${TEST} alut)
	endforeach()

	# copy over testdata, so test-suite can be used in binary dir
	set(TESTDATA file1.wav file2.au file3.raw)

	foreach(TESTDATUM ${TESTDATA})
		configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test_suite/${TESTDATUM}"
					   "${CMAKE_CURRENT_BINARY_DIR}/${TESTDATUM}" COPYONLY)
	endforeach()

	add_test(
		NAME errorstuff
		COMMAND test_errorstuff
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	add_test(
		NAME fileloader
		COMMAND test_fileloader
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	add_test(
		NAME memoryloader
		COMMAND test_memoryloader
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	add_test(
		NAME version
		COMMAND test_version
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	add_test(
		NAME waveforms
		COMMAND test_waveforms
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
