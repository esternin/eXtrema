dnl
dnl Check for the MUon Data (MUD) library. (See http://musr.org/mud/)
dnl The output variables MUD_CXXFLAGSS and MUD_LIB will contain the
dnl compiler flags and libraries necessary to use the MUD library,
dnl e.g.,
dnl   with-MUD=/usr/local/mud
dnl
AC_DEFUN([AX_CHECK_MUD], [
  AC_ARG_WITH(MUD,AC_HELP_STRING([--with-MUD(=DIR)],[use the MUon Data package (in DIR)]),,with_MUD=no)
  if test "$with_MUD" != "no" && test "$with_MUD" != ""; then
    MUD_CXXFLAGS="-DHAVE_MUD -I$with_MUD/src"
    MUD_LIB="-L$with_MUD/lib -lmud"
  fi
  AC_SUBST(MUD_CXXFLAGS)
  AC_SUBST(MUD_LIB)
])
