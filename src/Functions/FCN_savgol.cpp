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
#include "FCN_savgol.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_savgol *FCN_savgol::fcn_savgol_ = 0;

void FCN_savgol::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 0 )
      throw EExpressionError( wxT("SAVGOL: first argument must be a scalar") );
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    if( numberOfArguments_ == 1 )
    {
      if( ndmEff != 0 )
        throw EExpressionError( wxT("SAVGOL: second argument must be a scalar") );
      numberOfArguments_ = 2;
    }
    else if( numberOfArguments_ == 2 )
    {
      if( ndmEff != 1 )
        throw EExpressionError( wxT("SAVGOL: third argument must be a vector") );
      numberOfArguments_ = 3;
    }
  }
}

void FCN_savgol::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  std::size_t filterOrder;
  if( codes[0]->IsaNVariable() )
  {
    filterOrder = static_cast<std::size_t>(codes[0]->GetNVarPtr()->GetData().GetScalarValue());
  }
  else if( codes[0]->IsaWorkspace() )
  {
    filterOrder = static_cast<std::size_t>(codes[0]->GetWorkspacePtr()->GetValue());
  }
  else
  {
    filterOrder = static_cast<std::size_t>(codes[0]->GetConstantValue());
  }
  if( filterOrder!=2 && filterOrder!=4 )
    throw EExpressionError( wxT("SAVGOL: first argument must be 2 or 4") );
  //
  std::size_t filterWidth;
  if( codes[1]->IsaNVariable() )
  {
    filterWidth = static_cast<std::size_t>(codes[1]->GetNVarPtr()->GetData().GetScalarValue());
  }
  else if( codes[1]->IsaWorkspace() )
  {
    filterWidth = static_cast<std::size_t>(codes[1]->GetWorkspacePtr()->GetValue());
  }
  else
  {
    filterWidth = static_cast<std::size_t>(codes[1]->GetConstantValue());
  }
  //
  std::vector<double> yIn;
  std::size_t size;
  if( codes[2]->IsaNVariable() )
  {
    NumericData nd( codes[2]->GetNVarPtr()->GetData() );
    yIn.assign( nd.GetData().begin(), nd.GetData().end() );
    size = nd.GetDimMag(0);
  }
  else
  {
    Workspace *w = codes[2]->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("SAVGOL: third argument workspace is empty") );
    yIn.assign( w->GetData().begin(), w->GetData().end() );
    size = w->GetDimMag(0);
  }
  if( size < 2 )throw EExpressionError( wxT("SAVGOL: third argument length must be >= 2") );
  //
  std::vector<double> yOut( size, 0.0 );
  UsefulFunctions::SavitzkyGolay( yIn, yOut, filterWidth, filterOrder );
  ws->SetData( yOut );
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, size );
}

// end of file
