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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "OperatorDefinition.h"
#include "OperatorTable.h"

OperatorTable *OperatorTable::operatorTable_ = 0;

bool OperatorTable::Contains( OperatorDefinition const *opd ) const
{ return Contains( opd->Name() ); }

void OperatorTable::AddEntry( OperatorDefinition *opd )
{
  ExString name = opd->Name();
  if( Contains(name) )throw EExpressionError(
    ExString("The operator ")+name+ExString(" is already in the table") );
  operatorMap_->insert( entryType(name,opd) );
}

// end of file
