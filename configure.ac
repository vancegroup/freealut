# Process this file with autoconf to produce a configure script.

AC_INIT([freealut library], [1.0.0], [openal-devel@opensource.creative.com], [freealut])
AM_INIT_AUTOMAKE
AC_PREREQ([2.56])
AC_CONFIG_SRCDIR([AUTHORS])
AC_CONFIG_HEADERS([config.h])
AC_PREFIX_DEFAULT(/usr)

# Compatibility hack for older autoconf versions
m4_ifdef([AS_HELP_STRING], [], [AC_DEFUN([AS_HELP_STRING], [AC_HELP_STRING($][@)])])

# Checks for programs.
AC_PROG_CC
AC_C_CONST
AC_EXEEXT
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])


if echo $includedir | egrep "AL$" > /dev/null; then
    echo "includedir is" $includedir "libdir is" $libdir
else
    includedir="${includedir}/AL"
    echo "includedir changed to" $includedir "libdir is" $libdir
fi

# Checks for libraries.
AC_PATH_XTRA

if test x"$no_x" = xyes; then
  GL_LIBS="-lopenal32 -lpthread"
  LIBXXF86VM=
  VERSION_INFO=
  EXPORT_FLAGS="-DFREEALUT_EXPORTS"
else
  AL_LIBS="-lopenal -lpthread"
  EXPORT_FLAGS=
fi
AC_SUBST([AL_LIBS])
AC_SUBST([EXPORT_FLAGS])

# Checks for header files.
AC_HEADER_STDC
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CHECK_HEADERS([AL/al.h AL/alc.h stdio.h stdlib.h assert.h math.h sys/types.h sys/stat.h])
AC_HEADER_TIME
CPPFLAGS="$save_CPPFLAGS"

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_VPRINTF
AC_CHECK_LIBM
AC_SUBST([LIBM])

LIBRARY=alut
HEADER=alut.h
AC_SUBST([LIBRARY])
AC_SUBST([HEADER])

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
                 include/AL/Makefile
                 test_suite/Makefile
                 examples/Makefile])
AC_OUTPUT

