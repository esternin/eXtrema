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
#AC_INIT([extrema],[4.4.5],[Joseph L. Chuma joe.chuma@gmail.com])
AC_INIT([extrema],[4.6],[edward.sternin@brocku.ca],[extrema],[https://www.physics.brocku.ca/Labs/extrema/])

# use foreign to avoid "mandatory" NEWS and AUTHORS; automake syntax changed at v1.12
AM_INIT_AUTOMAKE([1.12 foreign])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_PROG_CC
AC_PROG_CXX
#LF_SET_WARNINGS

LT_INIT([disable-shared])

#AC_DISABLE_SHARED
#AC_PROG_LIBTOOL

#AX_CHECK_WX
#AX_CHECK_MUD
#AX_CHECK_MINUIT
AX_SETCXXFLAGS

# may need includes from other subdirectories in src/*  - ES
CXXFLAGS="$CXXFLAGS -I ../../../src -I ../../../src/Commands -I ../../../src/Expression -I ../../../src/Functions -I ../../../src/Graphics -I ../../../src/Operators -I ../../../src/Parsing -I ../../../src/Utilities -I ../../../src/Variables -I ../../../src/wxForms"

# check for the wxWidgets (from https://wiki.wxwidgets.org/Autoconf - ES)
#
# AM_OPTIONS_WXCONFIG adds options like --with-wx-config to enable using an
# uninstalled build of wxWidgets. See these options by running ./configure --help=short
AM_OPTIONS_WXCONFIG
reqwx=3.0.4
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"

# be sure to 'sudo apt-get install libwxgtk3.0-gtk3-dev' if this fails - ES
AC_SUBST(GTK_LIBS,`pkg-config --libs gtk+-3.0`)

LIBS="$LIBS $WX_LIBS $GTK_LIBS"

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
