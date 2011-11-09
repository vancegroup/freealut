set(VAR HAVE_VISIBILITY)

if(NOT DEFINED ${VAR})
	set(SOURCE
		"void __attribute__((visibility(\"default\"))) test() {}
#ifdef __INTEL_COMPILER
#error ICC breaks with binutils and visibility
#endif
int main(){}
")
	file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/src.c" "${SOURCE}")

	message(STATUS "Performing Test ${VAR}")
	try_compile(${VAR}
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/CMakeTmp/src.c
		CMAKE_FLAGS
		"-DCOMPILE_DEFINITIONS:STRING=-fvisibility=hidden"
		OUTPUT_VARIABLE
		OUTPUT)

	write_file(${CMAKE_BINARY_DIR}/CMakeOutput.log
		"Performing manual C SOURCE FILE Test ${VAR} with the following output:\n"
		"${OUTPUT}\n"
		"Source file was:\n${SOURCE}\n"
		APPEND)

	set(${VAR} ${${VAR}} CACHE INTERNAL "Test Visibility")
	if(${VAR})
		message(STATUS "Performing Test ${VAR} - Success")
	else()
		message(STATUS "Performing Test ${VAR} - Failed")
	endif()
endif()

if(${VAR})
	add_definitions(-fvisibility=hidden)
	add_definitions(-DHAVE_GCC_VISIBILITY)
endif()
