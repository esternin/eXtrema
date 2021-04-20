/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
 
#include "FCN_canopen.h"
 
FCN_canopen FCN_canopen::fcn_canopen_;

void FCN_canopen::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  nLoop = 0;
  numberOfArguments_ = 1;
}

void FCN_canopen::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExString name;
  if( arg1->IsaWorkspace() )name = arg1->GetWorkspacePtr()->GetFinalString();
  else if( arg1->IsaTString() )name = arg1->GetTString();
  else
    throw EExpressionError( "argument of CANOPEN function must be character" );
  //
  std::ifstream in;
  in.open( name.c_str(), ios_base::in );
  if( !in )ws->SetValue( 0.0 );
  else
  {
    in.close();
    ws->SetValue( 1.0 );
  }
}

// end of file
