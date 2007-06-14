# Copyright (C) 2006 Joseph L. Chuma <chuma@triumf.ca>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = Commands Expression Functions Graphics Operators Parsing Utilities Variables wxForms .

# EX_CXXFLAGS, WX_CXXFLAGS, MUD_CXXFLAGS, and MINUIT_CXXFLAGS  are defined in acinclude.m4
AM_CPPFLAGS = $(EX_CXXFLAGS) $(WX_CXXFLAGS) $(MUD_CXXFLAGS) $(MINUIT_CXXFLAGS)

AM_LDFLAGS = $(EX_LDFLAGS) $(WX_LIBS) $(GTK_LIBS)

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
extrema_LDADD = libextrema.la $(WX_LIBS) $(GTK_LIBS) $(MUD_LIB) $(MINUIT_LIB)

install-data-local:
	-cp -Rf ../Images $(pkgdatadir)/Images
	-cp -Rf ../Help $(pkgdatadir)/Help
