/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef FCN_ARRAYFUNCTION
#define FCN_ARRAYFUNCTION

#include "wx/wx.h"

#include "FunctionDefinition.h"

class Workspace;

class ArrayFunction : public FunctionDefinition
{
  // A virtual class for array functions
  
public:
  virtual ~ArrayFunction()
  {}
  
  virtual void ProcessArgument( bool, int, int & )
  {}
  
  virtual void ArrayEval( Workspace * )
  {}
  
protected:
  ArrayFunction( wxString const &name,
                 std::size_t minArg=1, std::size_t maxArg=1,
                 Type fcnType=NUM2NUMVECTOR,
                 ArgType argType1=NUMERIC, ArgType argType2=NUMERIC,
                 ArgType argType3=NUMERIC, ArgType argType4=NUMERIC,
                 ArgType argType5=NUMERIC, ArgType argType6=NUMERIC,
                 ArgType argType7=NUMERIC, ArgType argType8=NUMERIC,
                 ArgType argType9=NUMERIC, ArgType argType10=NUMERIC,
                 ArgType argType11=NUMERIC, ArgType argType12=NUMERIC,
                 ArgType argType13=NUMERIC, ArgType argType14=NUMERIC,
                 ArgType argType15=NUMERIC, ArgType argType16=NUMERIC,
                 ArgType argType17=NUMERIC, ArgType argType18=NUMERIC,
                 ArgType argType19=NUMERIC, ArgType argType20=NUMERIC )
      : FunctionDefinition( name, minArg, maxArg, fcnType,
                            argType1, argType2, argType3, argType4, argType5,
                            argType6, argType7, argType8, argType9, argType10,
                            argType11, argType12, argType13, argType14, argType15,
                            argType16, argType17, argType18, argType19, argType20 )
  {}
  
  int numberOfArguments_;
};

#endif
