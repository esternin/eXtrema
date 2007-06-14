# Copyright (C) 2006 Joseph L. Chuma <chuma@triumf.ca>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AM_CPPFLAGS = $(EX_CXXFLAGS) $(WX_CXXFLAGS) $(MUD_CXXFLAGS) $(MINUIT_CXXFLAGS)

noinst_LTLIBRARIES = libexpression.la
libexpression_la_SOURCES =  ExprCodes.cpp ExprCodes.h \
                            Expression.cpp Expression.h \
                            ListVector.cpp ListVector.h \
                            Workspace.cpp Workspace.h \
                            BndChk.h EExpressionError.h \
                            RangeVector.h
