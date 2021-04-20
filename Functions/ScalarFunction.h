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
#ifndef FCN_SCALARFUNCTION
#define FCN_SCALARFUNCTION

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "FunctionDefinition.h"

class Workspace;

class FUNCTIONS_LINKAGE_ ScalarFunction : public FunctionDefinition
{
  // A virtual class for scalar functions, i.e.,
  // functions that take a single numeric argument
  // and return a single numeric result
  
public:
  virtual ScalarFunction::~ScalarFunction()
  {}
  
  virtual void CalcArgument( bool, int, int *, Workspace * );

protected:
  ScalarFunction::ScalarFunction( char const *name,
                                  std::size_t minArg=1, std::size_t maxArg=1,
                                  Type fcnType=NUM2NUMSCALAR,
                                  ArgType argType1=NUMERIC, ArgType argType2=NUMERIC )
      : FunctionDefinition( name, minArg, maxArg, fcnType, argType1, argType2 )
  {}
  
};

#endif
