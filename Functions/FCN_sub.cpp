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

#include "ExString.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "TextVariable.h"
#include "EExpressionError.h"

#include "FCN_sub.h"

FCN_sub FCN_sub::fcn_sub_;

void FCN_sub::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    nLoop = 0;
    numberOfArguments_ = 1;
  }
  else
  {
    ++numberOfArguments_;
    nLoop = ndmEff;
  }
}

void FCN_sub::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  if( !arg1->IsaTString() || !arg2->IsaTString() )
    throw EExpressionError( "both arguments of SUB function must be character" );
  ExString s2( arg2->GetTString() );
  if( s2.find(arg1->GetTString(),0) != s2.npos() )
    ws->SetValue( 1.0 );    // true
  else
    ws->SetValue( 0.0 );    // false
}
 
// end of file
