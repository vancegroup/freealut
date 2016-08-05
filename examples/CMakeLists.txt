if(BUILD_EXAMPLES)
	# examples
	add_executable(hello_world hello_world.c)
	target_link_libraries(hello_world alut OpenSLES)

	add_executable(playfile playfile.c)
	target_link_libraries(playfile alut OpenSLES)
endif()
