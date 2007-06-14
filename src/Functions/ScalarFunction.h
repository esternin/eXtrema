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

#include "FunctionDefinition.h"

class Workspace;

class ScalarFunction : public FunctionDefinition
{
  // A virtual class for scalar functions, i.e.,
  // functions that take a single numeric argument
  // and return a single numeric result
  
public:
  virtual ~ScalarFunction()
  {}
  
  virtual void CalcArgument( bool, int, int *, Workspace * );

protected:
  ScalarFunction( wxString const &, std::size_t =1, std::size_t =1, Type =NUM2NUMSCALAR,
                  ArgType =NUMERIC, ArgType =NUMERIC );
};

#endif
