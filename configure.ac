# Copyright (C) 2005,...,2010 Joseph L. Chuma <joe.chuma@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ(2.59)

# touch src/wxForms/AnalysisWindow.cpp to get the latest version number
AC_INIT([extrema],[4.4.5],[Joseph L. Chuma joe.chuma@gmail.com])

# automake < 1.9 will not be allowed
# use gnu strictness
AM_INIT_AUTOMAKE([1.9])

LF_CONFIGURE_CC
LF_CONFIGURE_CXX
LF_SET_WARNINGS

AC_DISABLE_SHARED
AC_PROG_LIBTOOL

AX_CHECK_WX
AX_CHECK_MUD
AX_CHECK_MINUIT
AX_SETCXXFLAGS

AC_SUBST(GTK_LIBS,`pkg-config --libs gtk+-2.0`)

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
