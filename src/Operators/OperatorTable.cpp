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
#include "OperatorTable.h"

#include "OperatorDefinition.h"
#include "EExpressionError.h"

#include "OPR_and.h"
#include "OPR_concatenation.h"
#include "OPR_divide.h"
#include "OPR_equals.h"
#include "OPR_exclusiveOr.h"
#include "OPR_exponentiation2.h"
#include "OPR_exponentiation.h"
#include "OPR_ge.h"
#include "OPR_gt.h"
#include "OPR_innerProduct.h"
#include "OPR_intersection.h"
#include "OPR_le.h"
#include "OPR_lt.h"
#include "OPR_minus.h"
#include "OPR_multiply.h"
#include "OPR_ne.h"
#include "OPR_not.h"
#include "OPR_or.h"
#include "OPR_outerProduct.h"
#include "OPR_plus.h"
#include "OPR_reflect.h"
#include "OPR_transpose.h"
#include "OPR_union.h"

OperatorTable *OperatorTable::operatorTable_ = 0;

OperatorTable::OperatorTable()
{ operatorMap_ = new operatorMap; }

OperatorTable::OperatorTable( OperatorTable const &rhs )
{
  delete operatorMap_;
  operatorMap_ = new operatorMap(*(rhs.operatorMap_));
}

OperatorTable::~OperatorTable()
{ ClearTable(); }

OperatorTable *OperatorTable::GetTable()
{
  // returns the pointer to the OperatorTable object
  // OperatorTable is a "singleton", and can get its
  // pointer by this operator.  The first time this
  // operator is called, the OperatorTable object
  // is instantiated.
  //
  if( !operatorTable_ )operatorTable_ = new OperatorTable();
  return operatorTable_;
}

void OperatorTable::ClearTable()
{
  operatorMap::const_iterator end = operatorMap_->end();
  for( operatorMap::const_iterator i=operatorMap_->begin(); i!=end; ++i )delete (*i).second;
  delete operatorMap_;
}

bool OperatorTable::Contains( OperatorDefinition const *opd ) const
{ return Contains( opd->Name() ); }

void OperatorTable::AddEntry( OperatorDefinition *opd )
{
  wxString name = opd->Name();
  if( Contains(name) )throw EExpressionError(
    wxString(wxT("The operator "))+name+wxT(" is already in the table") );
  operatorMap_->insert( entryType(name,opd) );
}

OperatorDefinition *OperatorTable::GetOperator( wxString const &name ) const
{
  operatorMap::iterator i = operatorMap_->find( name );
  if( i != operatorMap_->end() )return (*i).second;
  return 0;
}

void OperatorTable::CreateOperators()
{
  OPR_and::Instance();
  OPR_concatenation::Instance();
  OPR_divide::Instance();
  OPR_equals::Instance();
  OPR_exclusiveOr::Instance();
  OPR_exponentiation2::Instance();
  OPR_exponentiation::Instance();
  OPR_ge::Instance();
  OPR_gt::Instance();
  OPR_innerProduct::Instance();
  OPR_intersection::Instance();
  OPR_le::Instance();
  OPR_lt::Instance();
  OPR_minus::Instance();
  OPR_multiply::Instance();
  OPR_ne::Instance();
  OPR_not::Instance();
  OPR_or::Instance();
  OPR_outerProduct::Instance();
  OPR_plus::Instance();
  OPR_reflect::Instance();
  OPR_transpose::Instance();
  OPR_union::Instance();
}

// end of file
