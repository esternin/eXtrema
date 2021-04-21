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
AC_INIT([extrema],[4.6],[edward.sternin@brocku.ca],[extrema],[https://www.physics.brocku.ca/Labs/extrema/])

# use foreign to avoid "mandatory" NEWS and AUTHORS; automake syntax changed at v1.12
AM_INIT_AUTOMAKE([1.12 foreign])

AC_PROG_CC
AC_PROG_CXX

dnl This is a workaround for the harmless but annoying warning
dnl
dnl ar: `u' modifier ignored since `D' is the default (see `U')
dnl
dnl given by Linux systems using recent binutils with the switch to building
dnl deterministic archives (that can't include the timestamps) when building
dnl all static libraries with default "cru" flags used by Libtool up to 2.4.6.
AR_FLAGS=cr

LT_INIT([disable-shared])

#AX_CHECK_MUD
#AX_CHECK_MINUIT

dnl Check for the wxWidgets.
dnl
dnl This macro adds options like --with-wx-config to enable using an
dnl uninstalled build of wxWidgets. See these options by running ./configure --help=short
WX_CONFIG_OPTIONS

dnl This macro will define WX_CXXFLAGS and WX_LIBS used in the makefiles if
dnl successful and abort with the failure message otherwise.
WX_CONFIG_CHECK([3.0.4],[],[],[base,xml,core,adv,html])

AC_CONFIG_FILES([
   Makefile
   src/Makefile
   src/Commands/Makefile
   src/Expression/Makefile
   src/Functions/Makefile
   src/Graphics/Makefile
   src/Operators/Makefile
   src/Parsing/Makefile
   src/Utilities/Makefile
   src/Variables/Makefile
   src/wxForms/Makefile
])

AC_OUTPUT
