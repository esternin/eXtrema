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
#ifndef FCN_VARTYPE
#define FCN_VARTYPE

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "ExString.h"
#include "FunctionDefinition.h"

class FUNCTIONS_LINKAGE_ FCN_vartype : public FunctionDefinition
{
public:
  static FCN_vartype *Definition()
  { return &fcn_vartype_; }
  
  void TextArrayEval( int const, std::vector<Workspace*> &,
                      std::vector<ExString> & ) const
  {}
  
private:
  FCN_vartype() : FunctionDefinition( "VARTYPE", 1, 2, MIXED2CHAR, CHARACTER )
  {}

  static FCN_vartype fcn_vartype_;
};
 
#endif
 
