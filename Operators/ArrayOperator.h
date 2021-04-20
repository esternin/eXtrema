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
#ifndef OPR_ARRAYOPERATOR
#define OPR_ARRAYOPERATOR

#define OPERATORS_LINKAGE_
/*
#if defined BUILDING_OPERATORS_DLL_
#define OPERATORS_LINKAGE_ __declspec(dllexport)
#else
#define OPERATORS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "OperatorDefinition.h"

class OPERATORS_LINKAGE_ ArrayOperator : public OperatorDefinition
{
  // A virtual class for array operators
public:
  virtual ~ArrayOperator()
  {}
  
protected:
  ArrayOperator( ExString const &name, int unaryPriority, int binaryPriority )
      : OperatorDefinition( name, unaryPriority, binaryPriority,
                            OperatorDefinition::ARRAY )
  {}
};

#endif
