macro(CHECK_INCLUDE_FILE_DEFINE HEADER VAR)
	check_include_file(${HEADER} ${VAR})
	if(${VAR})
		add_define("${VAR} 1")
	endif()
endmacro()

macro(CHECK_FUNCTION_DEFINE HEADERS FUNC PARAM VAR)
	check_c_source_compiles("
${HEADERS}
int main(){
${FUNC} ${PARAM};
}
"
		${VAR})
	if(${VAR})
		add_define("${VAR} 1")
	endif()
endmacro()


if(WIN32)
	check_include_file_define(windows.h HAVE_WINDOWS_H)
	if(HAVE_WINDOWS_H)
		check_function_define("#include <windows.h>" Sleep "(0)" HAVE_SLEEP)
	endif()
else()

	check_include_file_define(time.h HAVE_TIME_H)
	check_include_file_define(unistd.h HAVE_UNISTD_H)
	if(HAVE_TIME_H)
		check_function_define("#include <time.h>"
			nanosleep
			"((struct timespec*)0, (struct timespec*)0)"
			HAVE_NANOSLEEP)
	else()

		if(HAVE_UNISTD_H)
			check_function_define("#include <unistd.h>" usleep "(0)" HAVE_USLEEP)
		endif()
	endif()
endif()
