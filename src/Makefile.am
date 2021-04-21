# Copyright (C) 2007 Joseph L. Chuma <chuma@triumf.ca>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = Commands Expression Functions Graphics Operators Parsing Utilities Variables wxForms .

# Make sure AM_CXXFLAGS is used in all subdirectories too
export AM_CXXFLAGS = \
    -DEX_BINDIR='"$(bindir)"' \
    -DEX_DATADIR='"$(datadir)"' \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/Commands \
    -I$(top_srcdir)/src/Expression \
    -I$(top_srcdir)/src/Functions \
    -I$(top_srcdir)/src/Graphics \
    -I$(top_srcdir)/src/Operators \
    -I$(top_srcdir)/src/Parsing \
    -I$(top_srcdir)/src/Utilities \
    -I$(top_srcdir)/src/Variables \
    -I$(top_srcdir)/src/wxForms \
    $(WX_CXXFLAGS) $(MUD_CXXFLAGS) $(MINUIT_CXXFLAGS)

lib_LTLIBRARIES = libextrema.la
libextrema_la_SOURCES =
nodist_EXTRA_libextrema_la_SOURCES = dummy.cxx
libextrema_la_LIBADD = Commands/libcommands.la \
                       Expression/libexpression.la \
                       Functions/libfunctions.la \
                       Graphics/libgraphics.la \
                       Operators/liboperators.la \
                       Parsing/libparsing.la \
                       Utilities/libutilities.la \
                       Variables/libvariables.la \
                       wxForms/libwxforms.la

bin_PROGRAMS = extrema
extrema_SOURCES = extrema.cpp extrema.h
extrema_LDADD = libextrema.la $(WX_LIBS) $(MUD_LIB) $(MINUIT_LIB)

install-data-local:
	-cp -Rf ../Images $(pkgdatadir)/Images
	-cp -Rf ../Help $(pkgdatadir)/Help
