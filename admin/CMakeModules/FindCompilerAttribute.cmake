set(VAR HAVE_ATTRIBUTE)

check_c_source_compiles("void  foo (int bar __attribute__((unused)) ) { }
static void baz (void) __attribute__((unused));
static void baz (void) { }
int main(){}
"
	${VAR})
if(${VAR})
	add_define("HAVE___ATTRIBUTE__ 1")
endif()
