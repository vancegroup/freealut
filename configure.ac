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
ALUT_C__ATTRIBUTE__

ALUT_CHECK_FLAG([-Wno-deprecated-declarations],
                [alut_wno_deprecated_declarations=yes],
                [alut_wno_deprecated_declarations=no])
AM_CONDITIONAL([WNO_DEPRECATED_DECLARATIONS],
               [test x"$alut_wno_deprecated_declarations" = xyes])

AC_EXEEXT
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

AC_DEFINE([ALUT_BUILD_LIBRARY], [1], [Define to 1 if you want to build the ALUT DLL.])

# Checks for libraries. (not perfect yet)
AC_SEARCH_LIBS([pthread_self], [pthread])
AC_SEARCH_LIBS([alGetError], [openal32 openal])

# Checks for header files.

# We could possibly need struct timespec and random(), which are not ANSI.
AM_CFLAGS="-D_XOPEN_SOURCE=500"
AC_SUBST([AM_CFLAGS])

AC_HEADER_STDC
AC_CHECK_HEADERS([AL/alc.h AL/al.h basetsd.h ctype.h math.h stdio.h time.h windows.h])

# Checks for library functions.
ALUT_CHECK_FUNC([[@%:@include <time.h>]],
                [nanosleep], [[((struct timespec*)0, (struct timespec*)0)]])

ALUT_CHECK_FUNC([[@%:@include <unistd.h>]],
                [usleep], [[(0)]])

ALUT_CHECK_FUNC([[@%:@include <windows.h>]],
                [Sleep], [[(0)]])

ALUT_CHECK_FUNC([[@%:@include <sys/types.h>
                  @%:@include <sys/stat.h>
                  @%:@include <unistd.h>]],
                [stat], [[("", (struct stat*)0)]])

ALUT_CHECK_FUNC([[@%:@include <sys/types.h>
                  @%:@include <sys/stat.h>]],
                [_stat], [[("", (struct _stat*)0)]])

AC_CHECK_LIBM
AC_SUBST([LIBM])

# Build time configuration.

AC_ARG_ENABLE([warnings],
[AS_HELP_STRING([--enable-warnings],
                [enable pedantic compiler warnings @<:@default=yes@:>@])])

if test "x$enable_warnings" != xno; then
  AX_CFLAGS_WARN_ALL_ANSI([flags])
  AM_CFLAGS="$AM_CFLAGS $flags"
fi

AC_ARG_ENABLE([werror],
[AS_HELP_STRING([--enable-werror],
                [enable failure on all warnings @<:@default=no@:>@])])

if test "x$enable_werror" = xyes; then
  if test "x$enable_warnings" = xno; then
    AC_MSG_WARN([--enable-werror ignored because of --disable-warnings])
  elif test "x$GCC" != xyes; then
    AC_MSG_WARN([--enable-werror ignored because no GCC was detected])
  else
    AM_CFLAGS="$AM_CFLAGS -Werror"
  fi
fi

# Generate output.
AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 include/Makefile
                 src/Makefile
                 test_suite/Makefile])
AC_OUTPUT
