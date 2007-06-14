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
#include "FCN_exist.h"
#include "Workspace.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
 
FCN_exist *FCN_exist::fcn_exist_ = 0;

void FCN_exist::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  nLoop = 0;
  numberOfArguments_ = 1;
}

void FCN_exist::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg = codes[0];
  wxString name;
  if( arg->IsaWorkspace() )name = arg->GetWorkspacePtr()->GetFinalString();
  else if( arg->IsaTString() )name = arg->GetTString();
  else
    throw EExpressionError( wxT("argument of EXIST function must be character") );
  //
  name.UpperCase();
  if( NVariableTable::GetTable()->Contains(name) )
    ws->SetValue( 1.0 );  // true
  else if( TVariableTable::GetTable()->Contains(name) )
    ws->SetValue( 1.0 );
  else
    ws->SetValue( 0.0 );
}
 
// end of file
