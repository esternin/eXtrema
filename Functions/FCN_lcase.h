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
#ifndef FCN_LCASE
#define FCN_LCASE

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "FunctionDefinition.h"

class FUNCTIONS_LINKAGE_ FCN_lcase : public FunctionDefinition
{
public:
  static FCN_lcase *Definition()
  { return &fcn_lcase_; }
  
  void TextScalarEval( int j, std::vector<ExString> &sStack ) const
  { sStack[j].ToLower(); }

private:
  FCN_lcase() : FunctionDefinition( "LCASE", 1, 1, CHAR2CHAR, CHARACTER )
  {}
  
  static FCN_lcase fcn_lcase_;
};

#endif

