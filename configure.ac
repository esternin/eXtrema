# Copyright (C) 2020,...,2021 Edward Sternin <edward.sternin@brocku.ca>
# Copyright (C) 2005,...,2010 Joseph L. Chuma <joe.chuma@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ([2.59])

# autoreconf --install suggests placing aclocal into a subdirectory ES
AC_CONFIG_MACRO_DIRS([m4])

# touch src/wxForms/AnalysisWindow.cpp to get the latest version number
AC_INIT([extrema],[4.6.2],[edward.sternin@brocku.ca],[extrema],[https://www.physics.brocku.ca/Labs/extrema/])

# use foreign to avoid "mandatory" NEWS and AUTHORS; automake syntax changed at v1.12
AM_INIT_AUTOMAKE([1.12 foreign subdir-objects])

AC_ARG_ENABLE(debug,
             [AC_HELP_STRING([--enable-debug],
                             [compile with debug information and without optimizations])],
             [case "x${enableval}" in
                   x) debug_option=yes ;;
                xyes) debug_option=yes ;;
                 xno) debug_option=no ;;
                   *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
             esac], [debug_option=no])

if test "$debug_option" = "yes"; then
    DEBUG_FLAG="-g"
    NOOPT_FLAGS="-O0"

    CFLAGS="$CFLAGS $DEBUG_FLAG $NOOPT_FLAGS"
    CXXFLAGS="$CXXFLAGS $DEBUG_FLAG $NOOPT_FLAGS"
    LDFLAGS="$LDFLAGS $DEBUG_FLAG"
else
    dnl Turn off assert checking in standard library and also wxWidgets.
    CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

AC_PROG_CC
AC_PROG_CXX

dnl Define conditional variable for MSW-specific parts of our makefiles.
USE_MSW=0
case "${host}" in
    *-*-mingw* )
        USE_MSW=1
        ;;
esac

AM_CONDITIONAL(USE_MSW, [test "$USE_MSW" = 1])

if test "$USE_MSW" = 1; then
    AC_CHECK_TOOL([WINDRES], [windres], no)
fi

dnl This is a workaround for the harmless but annoying warning
dnl
dnl ar: `u' modifier ignored since `D' is the default (see `U')
dnl
dnl given by Linux systems using recent binutils with the switch to building
dnl deterministic archives (that can't include the timestamps) when building
dnl all static libraries with default "cru" flags used by Libtool up to 2.4.6.
AR_FLAGS=cr

LT_INIT([disable-shared])

AX_CHECK_MUD
AX_CHECK_MINUIT

dnl Check for the wxWidgets.
dnl
dnl This macro adds options like --with-wx-config to enable using an
dnl uninstalled build of wxWidgets. See these options by running ./configure --help=short
WX_CONFIG_OPTIONS

dnl This macro will define WX_CXXFLAGS and WX_LIBS used in the makefiles if
dnl successful and abort with the failure message otherwise.
WX_CONFIG_CHECK([3.1.5],[],[],[base,xml,core,adv,html])

AC_CONFIG_FILES([
   Makefile
   src/Makefile
])

AC_OUTPUT
