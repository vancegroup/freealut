if(CMAKE_COMPILER_IS_GNUCC)
	#ADD_DEFINITIONS(-std=c99)

	if(WARNINGS)
		set(ADD_WFLAGS "${ADD_WFLAGS} -Wall -ansi -pedantic -W")

		if(MORE_WARNINGS)
			set(ADD_WFLAGS
				"${ADD_WFLAGS} -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels -Winline -Wlong-long -Wmissing-declarations -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wwrite-strings")
		endif()

		# Should we use turn warnings into errors?
		if(USE_WERROR)
			set(ADD_WFLAGS "${ADD_WFLAGS} -Werror -pedantic-errors")
		endif()
	endif()

	if(OPTIMIZATION)
		set(ADD_CFLAGS "${ADD_CFLAGS} -O2 -finline-functions -ffast-math")
	endif()

	if(PROFILE)
		set(ADD_CFLAGS "${ADD_CFLAGS} -pg")
		set(ADD_LDFLAGS "-pg")
	else()

		if(OPTIMIZATION)
			# -pg and -fomit-frame-pointer are incompatible
			set(ADD_CFLAGS "${ADD_CFLAGS} -fomit-frame-pointer")
		endif()
	endif()
else()

	message("Your compiler isn't fully supported yet - no flags set.")
endif()
