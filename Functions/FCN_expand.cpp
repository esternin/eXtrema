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

#include <deque>

#include "Workspace.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "TextVariable.h"
#include "NumericData.h"
#include "OperatorDefinition.h"

#include "FCN_expand.h"

FCN_expand FCN_expand::fcn_expand_;

void FCN_expand::TextScalarEval( int j, std::vector<ExString> &sStack ) const
{
  ExString str( sStack[j] );
  try
  {
    while ( FillOut(str) );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  sStack[j] = str;
}

bool FCN_expand::FillOut( ExString &str ) const
{
  Workspace *ws = new Workspace(str);
  try
  {
    ws->ParseAndCheck( true );
  }
  catch( EExpressionError &e )
  {
    delete ws;
    throw;
  }
  ExString s;
  bool foundTV = false;
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  std::deque<ExprCodes*>::iterator end = codes.end();
  for( std::deque<ExprCodes*>::iterator i=codes.begin(); i!=end; ++i )
  {
    if( (*i)->IsaTVariable() )
    {
      NumericData *ia = 0;
      NumericData *ic = 0;
      s += "("+(*i)->GetTVarPtr()->GetString(ia,ic)+")";
      foundTV = true;
    }
    else if( (*i)->IsaFunction() )
    {
      s += (*i)->GetFcnPtr()->Name();
    }
    else if( (*i)->IsaNVariable() )
    {
      s += (*i)->GetNVarPtr()->GetName();
    }
    else if( (*i)->IsaTString() )
    {
      s += (*i)->GetTString();
    }
    else if( (*i)->IsaConstant() )
    {
      s += (*i)->GetConstantString();
    }
    else if( (*i)->IsaOperator() )
    {
      s += (*i)->GetOpPtr()->Name();
    }
    else if( (*i)->IsaOpeningBracket() )
    {
      s += '(';
    }
    else if( (*i)->IsaClosingBracket() )
    {
      s += ')';
    }
    else if( (*i)->IsaComma() )
    {
      s += ',';
    }
    else if( (*i)->IsaSemicolon() )
    {
      s += ';';
    }
    else if( (*i)->IsaHash() )
    {
      s += '#';
    }
    else if( (*i)->IsaAsterisk() )
    {
      s += '*';
    }
    else if( (*i)->IsaImmediateValue() )
    {
      s += (*i)->GetImmediateValue();
    }
  }
  str = s;
  delete ws;
  return foundTV;
}

