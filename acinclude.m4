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
dnl
dnl Check for the Minuit library. (See http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/)
dnl The output variables MINUIT_CXXFLAGSS and MINUIT_LIB will contain the
dnl compiler flags and libraries necessary to use the Minuit library,
dnl e.g.,
dnl   with-MINUIT=/usr/local
dnl
AC_DEFUN([AX_CHECK_MINUIT], [
  AC_ARG_WITH(MINUIT,AC_HELP_STRING([--with-MINUIT(=DIR)],[use the Minuit minimization package (in DIR)]),,with_MINUIT=no)
  if test "$with_MINUIT" != "no" && test "$with_MINUIT" != ""; then
    MINUIT_CXXFLAGS="-DHAVE_MINUIT -I$with_MINUIT/include/Minuit"
    MINUIT_LIB="-L$with_MINUIT/lib -llcg_Minuit"
  fi
  AC_SUBST(MINUIT_CXXFLAGS)
  AC_SUBST(MINUIT_LIB)
])
