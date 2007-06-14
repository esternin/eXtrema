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
#include "FCN_index.h"
#include "Workspace.h"
#include "EExpressionError.h"
#include "ExprCodes.h"

FCN_index *FCN_index::fcn_index_ = 0;

void FCN_index::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
    ++numberOfArguments_;
}

void FCN_index::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  if( !arg1->IsaTString() )
    throw EExpressionError( wxT("INDEX: first argument must be character") );
  if( !arg2->IsaTString() )
    throw EExpressionError( wxT("INDEX: second argument must be character") );
  wxString s1( arg1->GetTString() );
  wxString s2( arg2->GetTString() );
  std::size_t idx = s1.find( s2, 0 );
  idx==s1.npos ? idx=0 : ++idx;
  ws->SetNumberOfDimensions(0);
  ws->SetValue( static_cast<double>(idx) );
}

// end of file
