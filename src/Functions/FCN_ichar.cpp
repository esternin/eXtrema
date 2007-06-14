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
#include "FCN_ichar.h"
#include "Workspace.h"
#include "EExpressionError.h"
#include "ExprCodes.h"

FCN_ichar *FCN_ichar::fcn_ichar_ = 0;

void FCN_ichar::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  nLoop = 1;
  numberOfArguments_ = 1;
}

void FCN_ichar::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg = codes[0];
  wxString s;
  if( arg->IsaWorkspace() )
  {
    s = arg->GetWorkspacePtr()->GetFinalString();
  }
  else if( arg->IsaTString() )
  {
    s = arg->GetTString();
  }
  else
  {
    throw EExpressionError( wxT("CHAR: argument must be character") );
  }
  std::size_t len = s.size();
  for( std::size_t i=0; i<len; ++i )
  {
    wxChar c = s[i];
    int j = static_cast<int>(c);
    ws->SetData( i, static_cast<double>(j) );
  }
  ws->SetNumberOfDimensions( 1 );
  ws->SetDimMag( 0, len );
}

// end of file
