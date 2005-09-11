# Process this file with autoconf to produce a configure script.

AC_INIT([freealut library], [1.0.0], [openal-devel@opensource.creative.com], [freealut])
AC_CONFIG_AUX_DIR([admin])
AM_INIT_AUTOMAKE
AC_PREREQ([2.56])
AC_CONFIG_SRCDIR([AUTHORS])
AC_CONFIG_HEADERS([config.h])

# Compatibility hack for older autoconf versions
m4_ifdef([AS_HELP_STRING], [], [AC_DEFUN([AS_HELP_STRING], [AC_HELP_STRING($][@)])])

# Checks for programs.
AC_PROG_CC
AC_C_CONST
AC_EXEEXT
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

AC_DEFINE([_ALUTLIB], [1], [Define to 1 if you want to build the ALUT DLL.])

# We could separate this into a macro...
AC_CACHE_CHECK([whether the C compiler supports __attribute__],
  [alut_cv___attribute__],
  [AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
    void  foo (int bar __attribute__((unused)) ) { }
    static void baz (void) __attribute__((unused));
    static void baz (void) { }]])],
    [alut_cv___attribute__=yes],
    [alut_cv___attribute__=no])])
if test "$alut_cv___attribute__" = yes; then
  AC_DEFINE([HAVE___ATTRIBUTE__], [1], [Define to 1 if your C compiler supports __attribute__])
fi

# Checks for libraries. (not perfect yet)
AC_SEARCH_LIBS([pthread_self], [pthread])
AC_SEARCH_LIBS([alGetError], [openal32 openal])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([AL/alc.h AL/al.h math.h stdio.h time.h windows.h])

# Checks for library functions.
AC_CHECK_FUNCS([nanosleep usleep Sleep])
AC_CHECK_LIBM
AC_SUBST([LIBM])

# Build time configuration.
AC_ARG_ENABLE([warnings],
[AS_HELP_STRING([--enable-warnings],
                [use all gcc compiler warnings @<:@default=yes@:>@])])
if test "x$enable_warnings" != xno -a "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -Wall -pedantic -Werror"
fi

AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],
                [enable debugging code (for developers) @<:@default=no@:>@])])
if test "x$enable_debug" = xyes; then
  AC_DEFINE([_DEBUG], [1], [Define to 1 if you want to include debugging code.])
fi

# Generate output.
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 include/Makefile
                 test_suite/Makefile
                 examples/Makefile])
AC_OUTPUT

