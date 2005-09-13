# ALUT_C__ATTRIBUTE__
# -------------------
AC_DEFUN([ALUT_C__ATTRIBUTE__],
[AC_CACHE_CHECK([whether the C compiler supports __attribute__],
  [alut_cv___attribute__],
  [AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
    void  foo (int bar __attribute__((unused)) ) { }
    static void baz (void) __attribute__((unused));
    static void baz (void) { }]])],
    [alut_cv___attribute__=yes],
    [alut_cv___attribute__=no])])
if test "$alut_cv___attribute__" = yes; then
  AC_DEFINE([HAVE___ATTRIBUTE__], [1],
            [Define to 1 if your C compiler supports __attribute__.])
fi
])# ALUT_C__ATTRIBUTE__


# ALUT_CHECK_FUNC(PROLOGUE, FUNCTION, ARGUMENTS)
# -----------------------------------------
AC_DEFUN([ALUT_CHECK_FUNC],
[AS_VAR_PUSHDEF([alut_var], [alut_cv_func_$2])dnl
AC_CACHE_CHECK([for $2],
  alut_var,
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([$1], [$2 $3;])],
                  [AS_VAR_SET(alut_var, yes)],
                  [AS_VAR_SET(alut_var, no)])])
if test AS_VAR_GET(alut_var) = yes; then
  AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$2]), [1],
                    [Define to 1 if you have the `$2' function.])
fi
AS_VAR_POPDEF([alut_var])dnl
])# ALUT_CHECK_FUNC
